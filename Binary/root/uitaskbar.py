# Search

	net.SendChatPacket("/in_game_mall")

# Change

		if app.ENABLE_ITEMSHOP:
			interface = constInfo.GetInterfaceInstance()
			if interface != None:
				interface.OpenItemShopWindow()
		else:
			net.SendChatPacket("/in_game_mall")

