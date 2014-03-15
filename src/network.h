#ifndef _ilang_network
#define _ilang_network

/* This file contains the interfaces for working with other remote network nodes
 * this is being written at the same time as the modification because there has to
 *	 be a large amount of cross over between the two files
 *
 */

#include <string>

namespace ilang {
	namespace network {
		class Node {
			virtual void msgMaster(std::string);
		};

		class Master : public Node {
			virtual void msgMaster(std::string);
		};
	}
}

#endif // _ilang_network
