# Search

	m_peerList.clear();

# add after

#ifdef ENABLE_ITEMSHOP
	m_IShopManager.clear();
	m_IShopLogManager.clear();
#endif

# Search

	peer->EncodeWORD(0xffff);

# Add after

#ifdef ENABLE_ITEMSHOP
	SendItemShopData(peer);
#endif


# Search


		default:
			sys_err("Unknown header (header: %d handle: %d length: %d)", header, dwHandle, dwLength);

# Add before


#ifdef ENABLE_ITEMSHOP
		case HEADER_GD_ITEMSHOP:
			RecvItemShop(peer, dwHandle, data);
			break;
#endif

# Add


#ifdef ENABLE_ITEMSHOP
#include "buffer_manager.h"
tm& stringToRealTime(const std::string& strDateTime)
{
	struct tm t;
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
	if (sscanf(strDateTime.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec) == 6)
	{
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_isdst = 0;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = sec;
	}
	return t;
}
bool sortItemShop(const TIShopData& first, const TIShopData& second)
{
	return first.sellCount > second.sellCount;
}
bool CClientManager::InitializeItemShop()
{
	m_IShopManager.clear();
	char szQuery[64];
	snprintf(szQuery, sizeof(szQuery), "SELECT * FROM player.ishop_data");
	SQLMsg* pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);
	if (pMsg->Get()->uiNumRows != 0)
	{
		std::vector<TIShopData> m_sortCache;

		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TIShopData ishopData;

			BYTE categoryType, categorySubType;
			str_to_number(ishopData.id, row[col++]);
			str_to_number(categoryType, row[col++]);
			str_to_number(categorySubType, row[col++]);
			str_to_number(ishopData.itemVnum, row[col++]);
			str_to_number(ishopData.itemPrice, row[col++]);
			str_to_number(ishopData.discount, row[col++]);

			char eventTime[40];
			strlcpy(eventTime, row[col++], sizeof(eventTime));
			struct tm offerTimeTm = stringToRealTime(eventTime);
			time_t offerTime = mktime(&offerTimeTm);

			strlcpy(eventTime, row[col++], sizeof(eventTime));
			struct tm addedTimeTm = stringToRealTime(eventTime);
			time_t addedTime = mktime(&addedTimeTm);

			ishopData.offerTime = offerTime;
			if (ishopData.offerTime < 0)
				ishopData.offerTime = 0;
			ishopData.addedTime = addedTime;
			if (ishopData.addedTime < 0)
				ishopData.addedTime = 0;

			str_to_number(ishopData.sellCount, row[col++]);
			str_to_number(ishopData.week_limit, row[col++]);
			str_to_number(ishopData.month_limit, row[col++]);

			m_sortCache.emplace_back(ishopData);

			auto itType = m_IShopManager.find(categoryType);
			if (itType != m_IShopManager.end())
			{
				auto itSubType = itType->second.find(categorySubType);
				if (itSubType != itType->second.end())
					itSubType->second.emplace_back(ishopData);
				else
				{
					std::vector<TIShopData> m_vec;
					m_vec.emplace_back(ishopData);
					itType->second.emplace(categorySubType, m_vec);
				}
			}
			else
			{
				std::vector<TIShopData> m_vec;
				std::map<BYTE, std::vector<TIShopData>> m_map;
				m_vec.emplace_back(ishopData);
				m_map.emplace(categorySubType, m_vec);
				m_IShopManager.emplace(categoryType, m_map);

			}
		}

		if (m_sortCache.size())
		{
			std::stable_sort(m_sortCache.begin(), m_sortCache.end(), sortItemShop);
			if (m_sortCache[0].sellCount <= 0)
			{
				for (auto itCategory = m_IShopManager.begin(); itCategory != m_IShopManager.end(); ++itCategory)
				{
					if (itCategory->second.size())
					{
						for (auto itSubCategory = itCategory->second.begin(); itSubCategory != itCategory->second.end(); ++itSubCategory)
						{
							for (DWORD j = 0; j < itSubCategory->second.size(); ++j)
								itSubCategory->second[j].topSellingIndex = -1;
						}
					}
				}
			}
			else
			{
				for (DWORD j = 0; j < m_sortCache.size(); ++j)
				{
					DWORD id = m_sortCache[j].id;
					for (auto itCategory = m_IShopManager.begin(); itCategory != m_IShopManager.end(); ++itCategory)
					{
						if (itCategory->second.size())
						{
							for (auto itSubCategory = itCategory->second.begin(); itSubCategory != itCategory->second.end(); ++itSubCategory)
							{
								for (DWORD x = 0; x < itSubCategory->second.size(); ++x)
								{
									if (itSubCategory->second[x].id == id)
										itSubCategory->second[x].topSellingIndex = j + 1;
								}
							}
						}
					}
				}
			}
		}
	}
	itemShopUpdateTime = time(0);
	return true;
}
void CClientManager::SetDragonCoin(DWORD id, long long amount)
{
	char szQuery[84];
	snprintf(szQuery, sizeof(szQuery), "UPDATE account.account SET coins = %lld WHERE id = %d", amount, id);
	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
}

void CClientManager::ItemShopIncreaseSellCount(DWORD itemID, int itemCount)
{
	long long sellCount = 0;
	char szQuery[84];
	snprintf(szQuery, sizeof(szQuery), "SELECT sellCount FROM player.ishop_data WHERE id = %u", itemID);
	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(sellCount, row[0]);
	}
	//sellCount += itemCount;
	sellCount += 1;
	snprintf(szQuery, sizeof(szQuery), "UPDATE player.ishop_data SET sellCount = %lld WHERE id = %u", sellCount, itemID);
	std::unique_ptr<SQLMsg> pMsgLast(CDBManager::instance().DirectQuery(szQuery));
}
long long CClientManager::GetDragonCoin(DWORD id)
{
	char szQuery[84];
	snprintf(szQuery, sizeof(szQuery), "SELECT coins FROM account.account WHERE id = %d", id);
	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	long long dc = 0;
	str_to_number(dc, row[0]);
	return dc;
}
void CClientManager::RecvItemShop(CPeer* pkPeer, DWORD dwHandle, const char* data)
{
	const BYTE subIndex = *(BYTE*)data;
	data += sizeof(BYTE);
	if (subIndex == ITEMSHOP_LOG)
	{
		if (!pkPeer)
			return;

		const DWORD accountID = *(DWORD*)data;
		data += sizeof(DWORD);

		auto it = m_IShopLogManager.find(accountID);
		if (it == m_IShopLogManager.end())
		{
			char szQuery[84];
			snprintf(szQuery, sizeof(szQuery), "SELECT * FROM player.ishop_log WHERE accountID = %u", accountID);
			SQLMsg* pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);
			if (pMsg->Get()->uiNumRows != 0)
			{
				std::vector<TIShopLogData> m_vec;

				MYSQL_ROW row = NULL;
				for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
				{
					int col = 0;
					TIShopLogData ishopData;
					str_to_number(ishopData.accountID, row[col++]);
					strlcpy(ishopData.playerName, row[col++], sizeof(ishopData.playerName));
					strlcpy(ishopData.buyDate, row[col++], sizeof(ishopData.buyDate));
					str_to_number(ishopData.buyTime, row[col++]);
					strlcpy(ishopData.ipAdress, row[col++], sizeof(ishopData.ipAdress));
					str_to_number(ishopData.itemVnum, row[col++]);
					str_to_number(ishopData.itemCount, row[col++]);
					str_to_number(ishopData.itemPrice, row[col++]);

					m_vec.emplace_back(ishopData);
				}
				if (m_vec.size())
					m_IShopLogManager.emplace(accountID, m_vec);
			}
		}

		it = m_IShopLogManager.find(accountID);
		if (it == m_IShopLogManager.end())
		{
			int logCount = 0;
			pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(int));
			pkPeer->Encode(&subIndex, sizeof(BYTE));
			pkPeer->Encode(&logCount, sizeof(int));
		}
		else
		{
			int logCount = it->second.size();

			pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle,sizeof(BYTE)+sizeof(int)+(sizeof(TIShopLogData)*logCount));
			pkPeer->Encode(&subIndex, sizeof(BYTE));
			pkPeer->Encode(&logCount, sizeof(int));
			if(logCount)
				pkPeer->Encode(&it->second[0],sizeof(TIShopLogData)*logCount);
		}
	}
	else if (subIndex == ITEMSHOP_RELOAD)
	{
		InitializeItemShop();
		SendItemShopData(NULL, true);
	}
	else if (subIndex == ITEMSHOP_BUY)
	{
		const DWORD accountID = *(DWORD*)data;
		data += sizeof(DWORD);

		char playerName[CHARACTER_NAME_MAX_LEN + 1];
		thecore_memcpy(&playerName, data, sizeof(playerName));
		data += sizeof(playerName);

		char ipAdress[16];
		thecore_memcpy(&ipAdress, data, sizeof(ipAdress));
		data += sizeof(ipAdress);

		const int itemID = *(int*)data;
		data += sizeof(int);

		const int itemCount = *(int*)data;
		data += sizeof(int);

		const bool isLogOpen = *(bool*)data;
		data += sizeof(bool);

		if (m_IShopManager.size())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				if (it->second.size())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						if (itEx->second.size())
						{
							for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
							{
								const TIShopData& itemData = *itReal;
								if (itemData.id == itemID)
								{
									long long accountDragonCoin = GetDragonCoin(accountID);

									long long itemPrice = itemData.itemPrice * itemCount;
									if (itemData.discount > 0)
										itemPrice = long((float(itemPrice) / 100.0) * float(100 - itemData.discount));

									if (itemPrice > accountDragonCoin)
									{
										int returnType = 0;
										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
										pkPeer->Encode(&subIndex, sizeof(BYTE));
										pkPeer->Encode(&returnType, sizeof(BYTE));
										return;
									}

									if (itemData.week_limit > 0)
									{
										DWORD weekCount = 0;
										char szQuery[124];
										snprintf(szQuery, sizeof(szQuery), "SELECT itemCount FROM player.ishop_log WHERE buyDate > DATE_SUB(NOW(), INTERVAL 1 WEEK) and accountID = %u", accountID);
										SQLMsg* pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);
										if (pMsg->Get()->uiNumRows != 0)
										{
											MYSQL_ROW row = NULL;
											for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
											{
												int buyCount;
												str_to_number(buyCount, row[0]);
												weekCount += buyCount;
											}
										}
										if (weekCount >= itemData.week_limit || weekCount+itemCount > itemData.week_limit)
										{
											int returnType = 1;
											pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE) + sizeof(int));
											pkPeer->Encode(&subIndex, sizeof(BYTE));
											pkPeer->Encode(&returnType, sizeof(BYTE));
											pkPeer->Encode(&itemData.week_limit, sizeof(int));
											return;
										}
									}

									if (itemData.month_limit > 0)
									{
										DWORD monthCount = 0;
										char szQuery[124];
										snprintf(szQuery, sizeof(szQuery), "SELECT itemCount FROM player.ishop_log WHERE buyDate > DATE_SUB(NOW(), INTERVAL 1 MONTH) and accountID = %u", accountID);
										SQLMsg* pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);
										if (pMsg->Get()->uiNumRows != 0)
										{
											MYSQL_ROW row = NULL;
											for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
											{
												int buyCount;
												str_to_number(buyCount, row[0]);
												monthCount += buyCount;
											}
										}
										if (monthCount >= itemData.month_limit || monthCount+itemCount > itemData.month_limit)
										{
											int returnType = 2;
											pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE) + sizeof(int));
											pkPeer->Encode(&subIndex, sizeof(BYTE));
											pkPeer->Encode(&returnType, sizeof(BYTE));
											pkPeer->Encode(&itemData.month_limit, sizeof(int));
											return;
										}
									}

									ItemShopIncreaseSellCount(itemID, itemCount);

									SetDragonCoin(accountID, accountDragonCoin - itemPrice);
									DWORD newItemID = GetEventFlag("SPECIAL_ITEM_ID") + 1;
									SetEventFlag("SPECIAL_ITEM_ID", newItemID);
									char szQuery[512];
									snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.item (id, owner_id, window, count, vnum) VALUES(%u, %u, %d, %d, %d)", newItemID, accountID, 4, itemCount, itemData.itemVnum);
									delete CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);

									snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.ishop_log (accountID, playerName, buyDate, buyTime, ipAdress, itemVnum, itemCount, itemPrice) VALUES(%u, '%s', NOW(), %d, '%s', %u, %d, %lld)", accountID, playerName, time(0), ipAdress, itemData.itemVnum, itemCount, itemPrice);
									delete CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER);

									char       timeText[21];
									time_t     now = time(0);
									struct tm  tstruct = *localtime(&now);
									strftime(timeText, sizeof(timeText), "%Y-%m-%d %X", &tstruct);

									TIShopLogData logData;
									logData.accountID = accountID;
									strlcpy(logData.playerName, playerName, sizeof(logData.playerName));
									strlcpy(logData.buyDate, timeText, sizeof(logData.buyDate));
									logData.buyTime = time(0);
									strlcpy(logData.ipAdress, ipAdress, sizeof(logData.ipAdress));
									logData.itemVnum = itemData.itemVnum;
									logData.itemCount = itemCount;
									logData.itemPrice = itemPrice;

									auto it = m_IShopLogManager.find(accountID);
									if (it == m_IShopLogManager.end())
									{
										if (isLogOpen)
										{
											std::vector<TIShopLogData> m_vec;
											m_vec.emplace_back(logData);
											m_IShopLogManager.emplace(accountID, m_vec);
										}
									}
									else
									{
										it->second.emplace_back(logData);
									}

									int returnType = 3;
									int packetSize = sizeof(BYTE) + sizeof(BYTE) + sizeof(bool)+ sizeof(DWORD)+sizeof(int)+sizeof(long long);
									if (isLogOpen)
										packetSize+= sizeof(TIShopLogData);

									pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, packetSize);
									pkPeer->Encode(&subIndex, sizeof(BYTE));
									pkPeer->Encode(&returnType, sizeof(BYTE));
									pkPeer->Encode(&isLogOpen, sizeof(bool));
									pkPeer->Encode(&logData.itemVnum, sizeof(DWORD));
									pkPeer->Encode(&logData.itemCount, sizeof(int));
									pkPeer->Encode(&logData.itemPrice, sizeof(long long));
									if (isLogOpen)
										pkPeer->Encode(&logData, sizeof(TIShopLogData));

									return;
								}
							}
						}
					}
				}
			}
		}
	}
}
void CClientManager::SendItemShopData(CPeer* pkPeer, bool isPacket)
{
	TEMP_BUFFER buf;
	buf.reset();

	BYTE subIndex = ITEMSHOP_LOAD;
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&itemShopUpdateTime, sizeof(int));
	buf.write(&isPacket, sizeof(bool));
	int categoryTotalSize = m_IShopManager.size();
	buf.write(&categoryTotalSize, sizeof(int));

	if (categoryTotalSize)
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			BYTE categoryIndex = it->first;
			buf.write(&categoryIndex, sizeof(BYTE));

			BYTE categorySize = it->second.size();
			buf.write(&categorySize, sizeof(BYTE));

			if (categorySize)
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					BYTE categorySubIndex = itEx->first;
					buf.write(&categorySubIndex, sizeof(BYTE));

					BYTE categorySubSize = itEx->second.size();
					buf.write(&categorySubSize, sizeof(BYTE));

					if (categorySubSize)
						buf.write(itEx->second.data(), sizeof(TIShopData) * categorySubSize);
				}
			}
		}
	}
	if (pkPeer != NULL)
	{
		pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, 0, buf.size());
		pkPeer->Encode(buf.read_peek(), buf.size());
	}
	else
	{
		ForwardPacket(HEADER_DG_ITEMSHOP, buf.read_peek(), buf.size());
	}
}
#endif

