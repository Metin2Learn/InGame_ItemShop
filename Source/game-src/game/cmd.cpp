# Add like other command

#ifdef ENABLE_ITEMSHOP
ACMD(do_ishop);
#endif


#ifdef ENABLE_ITEMSHOP
	{ "ishop",	do_ishop,		0,		POS_DEAD,	GM_PLAYER },
#endif

