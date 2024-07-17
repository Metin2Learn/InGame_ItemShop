# Search

	case 'p':

# add before

#ifdef ENABLE_ITEMSHOP
		case 'z':
		{
			BYTE subIndex = ITEMSHOP_RELOAD;
			db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, 0, sizeof(BYTE));
			db_clientdesc->Packet(&subIndex, sizeof(BYTE));
		}
		break;
#endif
