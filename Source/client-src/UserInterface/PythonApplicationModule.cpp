# Searc

	PyModule_AddIntConstant(poModule, "CAMERA_STOP", CPythonApplication::CAMERA_STOP);

# Add after

#ifdef ENABLE_ITEMSHOP
	PyModule_AddIntConstant(poModule, "ENABLE_ITEMSHOP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ITEMSHOP", 0);
#endif

