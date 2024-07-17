# Search

public:

# add before

#ifdef ENABLE_ITEMSHOP
public:
	bool		InitializeItemShop();
	void		SendItemShopData(CPeer* pkPeer = NULL, bool isPacket = false);
	void		RecvItemShop(CPeer* pkPeer, DWORD dwHandle, const char* data);
	long long	GetDragonCoin(DWORD id);
	void		SetDragonCoin(DWORD id, long long amount);
	void		ItemShopIncreaseSellCount(DWORD itemID, int itemCount);
protected:
	int			itemShopUpdateTime;
	std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
	std::map<DWORD, std::vector<TIShopLogData>> m_IShopLogManager;
#endif