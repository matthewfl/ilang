#include "base.h"

TEST_CASE("basic map reduce", "[mapreduce][module]") {
	RUN_CODE(
					 a = object {
						 a = 1;
						 b = 2;
					 };
					 main = {
						 import("i.map").create(a).map({});
					 };
					 );
}


// TEST_CASE("more advance map reduce", "[mapreduce][module]") {
// 	RUN_CODE(
// from i import map

// data = object {
//         a: "a",
//         b: "b",
//         c: "c"
// };
// main = {
//         map.create(data).map({|key, value, emit|
//                 emit(value, [key, value]);
//         }).reduce({|key, values, emit|
//                 assert(key == values[0][0]);
//                 assert(values.length == 1);
//                 assert(values[0][0] == values[0][1]);
//         });
// };

// 					 );

// }
