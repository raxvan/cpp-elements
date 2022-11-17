
#include <ttf.h>
#include <iostream>
#include <array_view.h>
#include <auto_ptr.h>
#include <fixed_string.h>
#include <fixed_vector.h>
#include <flags.h>
#include <hash_string.h>
#include <lambda.h>
#include <lambda_traits.h>
#include <pointer.h>
#include <string_helpers.h>
#include <string_pool.h>
#include <string_utils.h>
#include <string_view.h>
#include <vecmap.h>
#include <allocators/custom_allocators.h>
#include <allocators/threaded_linear_allocator.h>
#include <allocators/scoped_allocator.h>
#include <pools/primitive_bucket_pool.h>
#include <pools/primitive_pool.h>

// using namespace cppe;

void test_threaded_linear_allocator()
{
	cppe::threaded_linear_safe_allocator<cppe::threaded_overflow_allocator> alc;

	std::size_t					index;
	std::array<std::thread, 32> threads;

	index = 0;
	for (auto& t : threads)
	{
		index++;
		t = std::thread([=, &alc]() {
			void* dst = alc.alloc(sizeof(std::size_t));
			std::memcpy(dst, &index, sizeof(std::size_t));
			TEST_ASSERT(alc.threaded_linear_allocator::owns(dst) == false);
			TEST_ASSERT(alc.owns(dst) == true);
		});
	}
	for (auto& t : threads)
		t.join();

	TEST_ASSERT(alc.capacity() == 0);

	alc.reserve_and_clear();

	TEST_ASSERT(alc.capacity() == sizeof(std::size_t) * threads.size());

	index = 2;
	for (auto& t : threads)
	{
		index++;
		t = std::thread([=, &alc]() {
			void* dst = alc.alloc(sizeof(std::size_t));
			std::memcpy(dst, &index, sizeof(std::size_t));
			TEST_ASSERT(alc.owns(dst) == true);
		});
	}
	// alc.intrusive_visit([&](cppe::overflow_allocator& alc) {
	//	TEST_ASSERT(alc.max_size() == 0);
	// });
	for (auto& t : threads)
		t.join();
	// alc.intrusive_visit([&](cppe::overflow_allocator& alc) {
	//	TEST_ASSERT(alc.max_size() == 0);
	// });
	TEST_ASSERT(alc.capacity() == sizeof(std::size_t) * threads.size());
	alc.clear();
	TEST_ASSERT(alc.capacity() == sizeof(std::size_t) * threads.size());
}

void test_stack_allocator()
{
	const std::size_t unit = sizeof(int);

	cppe::stack_allocator_buffer buffer;

	{
		cppe::stack_allocator alc(buffer);

		TEST_ASSERT(alc.alloc_unique(unit) == nullptr);
		TEST_ASSERT(alc.alloc_linear(unit) == nullptr);
	}

	buffer.resize(unit);

	auto test_1u = [&](cppe::stack_allocator& alc) {
		auto sz = alc.size();
		TEST_ASSERT(alc.alloc_linear(unit) != nullptr);
		TEST_ASSERT(alc.alloc_linear(unit) == nullptr);
		TEST_ASSERT(alc.size() == (sz + unit));
	};

	{
		cppe::stack_allocator alc(buffer);
		test_1u(alc);
	}

	{
		cppe::stack_allocator top(buffer);

		{
			cppe::stack_allocator alc(buffer);
			test_1u(alc);
		}

		TEST_ASSERT(top.size() == 0);
	}

	buffer.resize(unit * 2);

	auto test_2u_resize = [&](cppe::stack_allocator& alc) {
		auto sz = alc.size();
		TEST_ASSERT(alc.alloc_linear(unit) != nullptr);
		TEST_ASSERT(alc.size() == (sz + unit));

		TEST_ASSERT(alc.alloc_linear(unit * 3) == nullptr);
		TEST_ASSERT(alc.size() == (sz + unit));

		TEST_ASSERT(alc.alloc_linear(unit) != nullptr);
		TEST_ASSERT(alc.size() == (sz + unit * 2));

		alc.clear();
		TEST_ASSERT(alc.size() == 0);
	};

	{
		cppe::stack_allocator base(buffer);

		{
			cppe::stack_allocator alc(buffer);
			test_2u_resize(alc);
		}

		{
			cppe::stack_allocator top(buffer);

			TEST_ASSERT(top.alloc_unique(unit) != nullptr);

			{
				cppe::stack_allocator alc(buffer);
				test_1u(alc);
			}

			TEST_ASSERT(top.size() == unit);
			TEST_ASSERT(top.alloc_linear(unit) != nullptr);
			TEST_ASSERT(top.size() == unit * 2);
		}

		TEST_ASSERT(base.size() == 0);
		TEST_ASSERT(base.alloc_linear(unit) != nullptr);
		TEST_ASSERT(base.size() == unit);

		{
			cppe::stack_allocator top(buffer);

			base.clear();
			TEST_ASSERT(base.size() == 0);

			test_1u(top);
		}

		TEST_ASSERT(base.size() == 0);
	}
}

void test_lambda_buffer()
{
	using job_func_t = void();
	using builder_t = cppe::lambda_traits<job_func_t>;
	using job_handle_t = typename cppe::lambda_traits<job_func_t>::lambda_ptr;

	cppe::stack_allocator_buffer storage;
	storage.resize(128);

	cppe::stack_allocator scope_allocator(storage);

	ttf::instance_counter pivot;

	std::vector<job_handle_t> jobs;
	TEST_ASSERT(pivot.share() == 1);
	for (std::size_t i = 0; i < 3; i++)
	{
		ttf::instance_counter t(pivot);
		auto				  header = builder_t::build_unique_call(scope_allocator, [t, i]() {
			 cppe::fixed_string<16> str;
			 str.format("call: %d ", (int)i);
			 std::cout << t.share() << "/" << str.c_str() << std::endl;
						 });
		jobs.push_back(header);
	}
	TEST_ASSERT(pivot.share() == 4);
	std::cout << "start call/" << pivot.share() << std::endl;
	for (auto& j : jobs)
		j(); // execute jobs
	std::cout << "end call/" << pivot.share() << std::endl;
	TEST_ASSERT(pivot.share() == 1);
	jobs.clear();
	TEST_ASSERT(pivot.share() == 1);
	scope_allocator.clear();
}

void test_bucket_pool()
{
	using bucket_pool_t = cppe::primitive_bucket_pool<std::size_t>;
	using primitive_pool_t = cppe::primitive_pool<std::size_t>;

	bucket_pool_t					   bp;
	primitive_pool_t				   pp { 100 };
	std::vector<bucket_pool_t::handle> bhandles;
	std::vector<std::size_t*>		   phandles;

	for (std::size_t i = 0; i < 100; i++)
	{
		auto handle = bp.alloc();
		TTF_ASSERT(handle.ptr != nullptr);
		TTF_ASSERT(handle.get_index() == i);
		*handle.ptr = i;
		bhandles.push_back(handle);

		auto phandle = pp.alloc();
		TTF_ASSERT(phandle != nullptr);
		phandles.push_back(phandle);
	}

	auto erase = [&]() {
		for (std::size_t i = 0; i < 20; i++)
		{
			{
				std::size_t hi = (i * 61) % bhandles.size();
				bp.free(bhandles[hi]);
				bhandles.erase(bhandles.begin() + hi);
			}

			{
				std::size_t hi = (i * 61) % phandles.size();
				pp.free(phandles[hi]);
				phandles.erase(phandles.begin() + hi);
			}
		}
	};
	auto append = [&]() {
		for (std::size_t i = 0; i < 20; i++)
		{
			{
				auto handle = bp.alloc();
				TTF_ASSERT(handle.ptr != nullptr);
				TTF_ASSERT(handle.get_index() == *handle.ptr);
				bhandles.push_back(handle);
			}

			{
				auto handle = pp.alloc();
				TTF_ASSERT(handle != nullptr);
				phandles.push_back(handle);
			}
		}
	};

	for (std::size_t i = 0; i < 20; i++)
	{
		erase();
		append();
	}

	for (auto h : bhandles)
		bp.free(h);

	for (auto h : phandles)
		pp.free(h);
}

void core_test_main()
{
	TEST_FUNCTION(test_threaded_linear_allocator);
	TEST_FUNCTION(test_stack_allocator);
	TEST_FUNCTION(test_lambda_buffer);
	TEST_FUNCTION(test_bucket_pool);
}

TEST_MAIN(core_test_main);
