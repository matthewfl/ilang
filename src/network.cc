#include "network.h"
#include <iostream>
using namespace std;


#define BOOST_ASIO_SEPARATE_COMPILATION

#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/extensions.hpp>

#include <libtorrent/peer_connection.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/peer_info.hpp>

namespace {
	using namespace libtorrent;
	struct ilang_peer_plugin : libtorrent::peer_plugin {
		int message_slot;
		ilang_peer_plugin() {
			message_slot = -1;
		}
		virtual void add_handshake (entry &e) {
			entry &message = e["m"];
			message["LT_ILANG"] = 0xa1;
			message["ILANG_VERSION"] = ILANG_VERSION;
		}
		virtual bool on_extension_handshake (const lazy_entry &e) {
			if(e.type() != lazy_entry::dict_t) {
				throw 1; // this should close the connection
			}
			lazy_entry const *message = e.dict_find("m");
			if(!message || message->type() != lazy_entry::dict_t) {
				throw 1; // should close connection
			}
			int index = message->dict_find_int_value("LT_ILANG", -1);
			if(index == -1) {
				throw 1; // close connection
			}
			if( message->dict_find_string_value("ILANG_VERSION") != ILANG_VERSION) {
				cerr << "Found node running wrong version\n";
				throw 1;
			}

			message_slot = index;
			return true;
		}
	};
	struct ilang_torrent_plugin : libtorrent::torrent_plugin {
		virtual ~ilang_torrent_plugin() {}
		virtual boost::Handle<peer_plugin> new_connection(peer_connection *con) {
			return boost::Handle<peer_plugin>(new ilang_peer_plugin);
		}
	};
	struct ilang_plugin : libtorrent::plugin {
		virtual ~ilang_plugin() {};
		virtual boost::Handle<torrent_plugin> new_torrent(torrent *t, void *user) {
			return boost::Handle<torrent_plugin>(new ilang_torrent_plugin);
		}
	};

}

namespace ilang {
	namespace network {
		void Node::msgMaster(std::string str) {

		}
		void Master::msgMaster(std::string str) {

		}
	}
}
