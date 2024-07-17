# Search

public:

# add before

#ifdef ENABLE_ITEMSHOP
public:
	void	LoadItemShopData(const char* c_pData);
	void	LoadItemShopData(LPCHARACTER ch, bool isAll = true);
	void	LoadItemShopLog(LPCHARACTER ch);
	void	LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData);
	void	LoadItemShopBuy(LPCHARACTER ch, int itemID, int itemCount);
	void	LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData);
	int		GetItemShopUpdateTime() { return itemshopUpdateTime; }

protected:
	int		itemshopUpdateTime;
	std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
#endif
