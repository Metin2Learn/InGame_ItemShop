# Search

	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

# add after

#ifdef ENABLE_ITEMSHOP
	if (!InitializeItemShop())
	{
		sys_err("InitializeItemShop FAILED");
		return false;
	}
#endif