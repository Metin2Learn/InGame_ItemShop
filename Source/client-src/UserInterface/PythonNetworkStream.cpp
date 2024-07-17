# Search

	Set(HEADER_GC_SCRIPT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCScript), DYNAMIC_SIZE_PACKET));

# Add after

#ifdef ENABLE_ITEMSHOP
		Set(HEADER_GC_ITEMSHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemShop), DYNAMIC_SIZE_PACKET));
#endif
