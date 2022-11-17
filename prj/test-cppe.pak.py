

def configure(cfg):
	t = cfg.link("../../ttf/testing.pak.py")
	t.enable()

	cfg.link("cppe.pak.py")


def construct(ctx):
	
	ctx.config("type","exe")

	ctx.fscan("src: ../test")

