# Search

	HEADER_GC_PARTY_UNLINK = 92,

# Add after

#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP = 93,
#endif

# Add


#ifdef ENABLE_ITEMSHOP
typedef struct SPacketGCItemShop
{
	BYTE	header;
	DWORD	size;
} TPacketGCItemShop;
#endif

