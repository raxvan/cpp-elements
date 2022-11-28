
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
#include <allocators/linear_allocator.h>
#include <allocators/scoped_allocator.h>
#include <pools/primitive_bucket_pool.h>
#include <pools/primitive_pool.h>
#include <pools/abstract_pool.h>

// using namespace cppe;

void test_threaded_linear_allocator()
{
	cppe::safe_linear_allocator<cppe::threaded_linear_allocator,cppe::threaded_overflow_allocator> alc;

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
	using bucket_pool_t = cppe::primitive_bucket_pool<std::size_t, 3>;
	using primitive_pool_t = cppe::primitive_pool<std::size_t>;

	bucket_pool_t					   bp;
	primitive_pool_t				   pp { 100 };
	std::vector<bucket_pool_t::handle> bhandles;
	std::vector<std::size_t*>		   phandles;

	for (std::size_t i = 0; i < 100; i++)
	{
		auto handle = bp.create();
		TTF_ASSERT(handle.ptr != nullptr);
		TTF_ASSERT(handle.get_debug_index() == i);
		*handle.ptr = i;
		bhandles.push_back(handle);

		auto phandle = pp.create();
		TTF_ASSERT(phandle != nullptr);
		phandles.push_back(phandle);
	}

	auto erase = [&]() {
		for (std::size_t i = 0; i < 20; i++)
		{
			{
				std::size_t hi = (i * 61) % bhandles.size();
				if (i % 2 == 0)
					bp.release(bhandles[hi]);
				else
					bp.release(bhandles[hi].ptr);
				bhandles.erase(bhandles.begin() + hi);
			}

			{
				std::size_t hi = (i * 61) % phandles.size();
				pp.release(phandles[hi]);
				phandles.erase(phandles.begin() + hi);
			}
		}
	};
	auto append = [&]() {
		for (std::size_t i = 0; i < 20; i++)
		{
			{
				auto handle = bp.create();
				TTF_ASSERT(handle.ptr != nullptr);
				TTF_ASSERT(handle.get_debug_index() == *handle.ptr);
				bhandles.push_back(handle);
			}

			{
				auto handle = pp.create();
				TTF_ASSERT(handle != nullptr);
				phandles.push_back(handle);
			}
		}
	};
	{
		std::size_t index = 0;
		bp.visit_objects([&](std::size_t i) {
			TTF_ASSERT(index == i);
			index++;
		});
	}
	for (std::size_t i = 0; i < 20; i++)
	{
		erase();
		append();
	}
	{
		std::size_t index = 0;
		bp.visit_objects([&](std::size_t i) {
			TTF_ASSERT(index == i);
			index++;
		});
	}

	for (auto h : bhandles)
		bp.release(h);

	bp.validate_empty();
	for (std::size_t i = 0; i < 20; i++)
		bp.create();
	bp.clear();
	bp.validate_empty();

	for (auto h : phandles)
		pp.release(h);
}

void test_abstract_pool()
{
	using allocator = cppe::safe_linear_allocator<cppe::linear_allocator, cppe::overflow_allocator>;
	
	cppe::AbstractPool<allocator> pool;

	TTF_ASSERT(pool.size() == 0);

	ttf::instance_counter test_instance;
	struct test_obj : public cppe::AbstractPoolEntry
	{
		ttf::instance_counter inst; 
	};

	{
		pool.set_capacity(sizeof(test_obj));

		auto* inst1 = pool.create<test_obj>();
		TTF_ASSERT(inst1 != nullptr);
		inst1->inst = test_instance;

		TTF_ASSERT(pool.size() == sizeof(test_obj));
		TTF_ASSERT(test_instance.share() == 2);

		auto* inst2 = pool.create<test_obj>();
		TTF_ASSERT(inst2 != nullptr);
		inst2->inst = test_instance;

		TTF_ASSERT(pool.size() == sizeof(test_obj) * 2);
		TTF_ASSERT(test_instance.share() == 3);

		pool.release(inst1);
		pool.release(inst2);

		TTF_ASSERT(test_instance.share() == 1);
	}

	{
		pool.clear();
		TTF_ASSERT(test_instance.share() == 1);

		auto* inst1 = pool.create<test_obj>();
		TTF_ASSERT(inst1 != nullptr);
		inst1->inst = test_instance;
		auto* inst2 = pool.create<test_obj>();
		TTF_ASSERT(inst2 != nullptr);
		inst2->inst = test_instance;
		TTF_ASSERT(test_instance.share() == 3);
		pool.clear();
		TTF_ASSERT(test_instance.share() == 1);
		TTF_ASSERT(pool.size() == 0);
	}

	{
		struct base_obj
		{
			int some_data{ 0 };
			virtual ~base_obj()
			{

			}
		};

		struct test_obj1 : public base_obj, public cppe::AbstractPoolEntry
		{
		};
		struct test_obj2 : public base_obj, public cppe::AbstractPoolEntryEx
		{
		};

		pool.set_capacity(1);
		auto* inst1 = pool.create<test_obj1>();
		TTF_ASSERT(inst1 != nullptr);
		pool.release(inst1);
		TTF_ASSERT(pool.size() != 0);//because it can't remove test_obj1 because allocator does not search properly in the alloc list
		pool.clear();


		auto* inst2 = pool.create<test_obj2>();
		pool.release(inst2);
		TTF_ASSERT(pool.size() == 0);
	}

}

void test_virtual_lambda()
{

	struct BaseInterface
	{
		virtual int operator ()(const int& value) = 0;
	};
	auto test_interface = [](BaseInterface& i) {
		int r = i(10);
		TTF_ASSERT(r == 20);
	};

	ttf::instance_counter s1;
	ttf::instance_counter s2;
	{
		auto double_the_value = cppe::lambda_traits<int(const int&)>::make_virtual_lambda<BaseInterface>([s1, s = std::move(s2)](const int& value) -> int {
			TTF_ASSERT(s1.share() == 2);
			TTF_ASSERT(s.share() == 1);
			return value * 2;
		});
		
		{
			auto f = std::move(double_the_value);
			test_interface(f);
		}

		TTF_ASSERT(s1.share() == 1);
	}


}

void core_test_main()
{
	TEST_FUNCTION(test_threaded_linear_allocator);
	TEST_FUNCTION(test_stack_allocator);
	TEST_FUNCTION(test_lambda_buffer);
	TEST_FUNCTION(test_bucket_pool);
	TEST_FUNCTION(test_abstract_pool);
	TEST_FUNCTION(test_virtual_lambda);

}

TEST_MAIN(core_test_main);
