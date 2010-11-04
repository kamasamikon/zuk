/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __PLAYLISTDEFINE_H__
#define __PLAYLISTDEFINE_H__

#include <ktypes.h>

#define PLC_DIRTY                   0x00000001	/**< not sync to ui */
#define PLC_DELETED                 0x00000002	/**< is deleted */
#define PLC_PRG_CHG                 0x00000004	/**< program belong to this channel changed */
#define PLC_FROZE                   0x00000008	/**< The device or channel forze */
#define PLC_SCANNING                0x00000010	/**< device scanning */

// 这个是需要保存的
typedef struct _KPlayListChannel {
	K_dlist_entry entry;    /**< ch_hdr */
	kuint flg;			/**< changed after last sync to playlist.html */
	kchar devhash[33];	    /**< which device belongs to */
	kchar hash[33];
	kchar name[128];	    /**< channel name */
	kchar desc[256];	    /**< channel desc, as hint */
	kchar logo[260];
	K_dlist_entry prg_hdr;	/**< all program belong to this channel */
} KPlayListChannel;

// 这个是随程序运行动态添加的
typedef struct _KPlayListProg {
	K_dlist_entry entry;    /**< will queue to KPlayListChannel::prg_hdr */
	kuint flg;			/**< changed after last sync to playlist.html */
	kchar chHash[33];	    /**< which channel belongs to */
	kchar name[128];
	kchar desc[256];
	time_t stime, etime;
} KPlayListProg;

typedef struct _KPlayListDevice {
	K_dlist_entry entry;
	kuint flg;			/**< changed after last sync to playlist.html */
	kchar hash[33];
	kchar name[256];
} KPlayListDevice;

class CPlayListRunTimeInfo {
public:
	CPlayListRunTimeInfo() {
		autoPlay = 1;
		loopPlay = 1;
		randomPlay = 0;
		playState = 1;
		volume = 50;
		mute = 0;

		prot_cnt = 0;
		dev_all = dev_use = 0;
		ch_all = ch_use = 0;

		curChannelHash[0] = '\0';

		im = NULL;
		mc = NULL;
		ch = NULL;

		init_dlist_head(&prot_hdr);
		init_dlist_head(&dev_hdr);
		init_dlist_head(&ch_hdr);
	} KMediaChannel *curChannel() {
		if (NULL == ch) {
			if (NULL == mc) {
				mc = (KMediaContainer *) kim_getptr(im, "p.sys.kmc", NULL);
			}
			if (mc) {
				if (ch == NULL) {
					ch = mc->getMediaChannelFromChannel((char *)
							curChannelHash);
				}
			}
		}
		return ch;
	}
	KMediaChannel *channelGoNext() {
		K_dlist_entry *entry;

		entry = ch_hdr.next;
		while (entry != &ch_hdr) {
			KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel, entry);
			if (0 == kstr_icmp((char *) curChannelHash, (char *) ch->hash, sizeof(ch->hash) - 1)) {
				entry = entry->next;
				break;
			}
			entry = entry->next;
		}
		if (entry != &ch_hdr) {
			KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel, entry);
			return setCurChannel(ch->hash);
		} else if (loopPlay) {
			// 到头了
			entry = entry->next;
			if (entry != &ch_hdr) {
				KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel,
						entry);
				return setCurChannel(ch->hash);
			}
		}
		return curChannel();
	}
	KMediaChannel *channelGoPrev() {
		K_dlist_entry *entry;

		entry = ch_hdr.prev;
		while (entry != &ch_hdr) {
			KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel, entry);
			if (0 == kstr_icmp((char *) curChannelHash, (char *) ch->hash, sizeof(ch->hash) - 1)) {
				entry = entry->prev;
				break;
			}
			entry = entry->prev;
		}
		if (entry != &ch_hdr) {
			KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel, entry);
			return setCurChannel(ch->hash);
		} else if (loopPlay) {
			// 到头了
			entry = entry->prev;
			if (entry != &ch_hdr) {
				KPlayListChannel *ch = FIELD_TO_STRUCTURE(entry, KPlayListChannel,
						entry);
				return setCurChannel(ch->hash);
			}
		}
		return curChannel();
	}

	KMediaChannel *setCurChannel(char *chHash) {
		strncpy((char *) curChannelHash, chHash, sizeof(curChannelHash) - 1);
		curChannelHash[sizeof(curChannelHash) - 1] = '\0';
		this->ch = NULL;
		return curChannel();
	}

public:
	// global
	int autoPlay;
	int loopPlay;
	int randomPlay;

	// play
	int playState;
	int volume;
	int mute;
	int playSpeed;

	char curChannelHash[33];

	KIM *im;
	KMediaContainer *mc;
	KMediaChannel *ch;

	int prot_cnt;
	K_dlist_entry prot_hdr;	// protocol

	int dev_all, dev_use;
	K_dlist_entry dev_hdr;	// dev

	int ch_all, ch_use;
	K_dlist_entry ch_hdr;	// channel
};

#endif
