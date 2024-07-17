# Add

if app.ENABLE_ITEMSHOP:
	import uiItemShop

# Search

		self.interface = interfaceModule.Interface()

# Add after

		if app.ENABLE_ITEMSHOP:
			constInfo.SetInterfaceInstance(self.interface)

# Search

		self.KillFocus()

# Add after

		if app.ENABLE_ITEMSHOP:
			constInfo.SetInterfaceInstance(None)

# Add

	if app.ENABLE_ITEMSHOP:
		def ItemShopClear(self, updateTime):
			uiItemShop.ItemShopClear(int(updateTime))
		def ItemShopAppendItem(self, categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount):
			uiItemShop.ItemShopAppendItem(int(categoryIndex), int(categorySubIndex), int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount))
		def ItemShopHideLoading(self):
			self.interface.ItemShopHideLoading()
		def ItemShopOpenMainPage(self):
			self.interface.OpenItemShopMainWindow()
		def ItemShopLogClear(self):
			uiItemShop.ItemShopLogClear()
		def ItemShopAppendLog(self, dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
			uiItemShop.ItemShopAppendLog(str(dateText), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))
		def ItemShopPurchasesWindow(self):
			self.interface.ItemShopPurchasesWindow()
		def ItemShopSetDragonCoin(self, dragonCoin):
			self.interface.ItemShopSetDragonCoin(long(dragonCoin))

