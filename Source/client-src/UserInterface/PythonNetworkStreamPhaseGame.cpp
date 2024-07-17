# Search

		case HEADER_GC_HANDSHAKE_OK:
			RecvHandshakeOKPacket();
			return;
			break;

# Add after

#ifdef ENABLE_ITEMSHOP
		case HEADER_GC_ITEMSHOP:
			ret = RecvItemShop();
			break;
#endif


# Add


#ifdef ENABLE_ITEMSHOP
bool CPythonNetworkStream::RecvItemShop()
{
	TPacketGCItemShop p;
	if (!Recv(sizeof(TPacketGCItemShop), &p))
		return false;

	BYTE subIndex;
	if (!Recv(sizeof(BYTE), &subIndex))
		return false;

	switch (subIndex)
	{
		case ITEMSHOP_DRAGONCOIN:
		{
			long long dragonCoin;
			if (!Recv(sizeof(long long), &dragonCoin))
				return false;
			bool isLogOpen;
			if (!Recv(sizeof(bool), &isLogOpen))
				return false;

			if (isLogOpen)
			{
				TIShopLogData logData;
				if (!Recv(sizeof(TIShopLogData), &logData))
					return false;
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog", Py_BuildValue("(sissiiL)", logData.buyDate, logData.buyTime, logData.playerName, logData.ipAdress, logData.itemVnum, logData.itemCount, logData.itemPrice));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin", Py_BuildValue("(L)", dragonCoin));
		}
		break;
		case ITEMSHOP_LOG:
		{
			int logCount;
			if (!Recv(sizeof(int), &logCount))
				return false;
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading", Py_BuildValue("()"));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopLogClear", Py_BuildValue("()"));
			if (logCount)
			{
				std::vector<TIShopLogData> m_vec;
				m_vec.resize(logCount);
				if (!Recv(sizeof(TIShopLogData)*logCount, &m_vec[0]))
					return false;
				for (DWORD j = 0; j < m_vec.size(); ++j)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog", Py_BuildValue("(sissiiL)", m_vec[j].buyDate, m_vec[j].buyTime, m_vec[j].playerName, m_vec[j].ipAdress, m_vec[j].itemVnum, m_vec[j].itemCount, m_vec[j].itemPrice));
			}
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopPurchasesWindow", Py_BuildValue("()"));
		}
		break;
		case ITEMSHOP_LOAD:
		{
			long long dragonCoin;
			if (!Recv(sizeof(long long), &dragonCoin))
				return false;
			int updateTime;
			if (!Recv(sizeof(int), &updateTime))
				return false;
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin", Py_BuildValue("(L)", dragonCoin));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading", Py_BuildValue("()"));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopClear", Py_BuildValue("(i)", updateTime));

			int categoryTotalSize;
			if (!Recv(sizeof(int), &categoryTotalSize))
				return false;

			if (categoryTotalSize == 9999)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage", Py_BuildValue("()"));
				return true;
			}

			for (DWORD j = 0; j < categoryTotalSize; ++j)
			{
				BYTE categoryIndex, categorySize;
				if (!Recv(sizeof(BYTE), &categoryIndex))
					return false;
				if (!Recv(sizeof(BYTE), &categorySize))
					return false;
				for (DWORD x = 0; x < categorySize; ++x)
				{
					BYTE categorySubIndex, categorySubSize;
					if (!Recv(sizeof(BYTE), &categorySubIndex))
						return false;
					if (!Recv(sizeof(BYTE), &categorySubSize))
						return false;

					std::vector<TIShopData> m_vec;
					if (categorySubSize)
					{
						m_vec.resize(categorySubSize);
						if (!Recv(sizeof(TIShopData) * categorySubSize, &m_vec[0]))
							return false;
						for (DWORD k = 0; k < m_vec.size(); ++k)
						{
							const TIShopData& shopData = m_vec[k];
							PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem", Py_BuildValue("(iiiiLiiiiL)",
								categoryIndex, categorySubIndex,  shopData.id, shopData.itemVnum, shopData.itemPrice, shopData.discount, shopData.offerTime, shopData.topSellingIndex, shopData.addedTime,
								shopData.sellCount));
						}
					}
				}
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage", Py_BuildValue("()"));
			}
		}
		break;
	}
	return true;
}
#endif

