#ifndef APPLE_FUNCTIONS_H_
#define APPLE_FUNCTIONS_H_

// ----------------------------------------------------------------------------
//     All Apple dependencies must be isolated here.  The long term goal is to
// remove all Objective-C and/or Swift code from this project.
// ----------------------------------------------------------------------------

#ifdef __APPLE__

#include <iostream>
#include <vector>
#include <string>
#import <Foundation/Foundation.h>

#include "hash_data.h"


int hashDataFromPlist(std::string filename, std::vector<HashData> *hashes);
int hashDataFromPlist(std::string filename, HashData *someHashData);
void dictionaryToHashData(NSDictionary *theDic, HashData *someHashData);
void rawShadowHashData(std::string user);

#endif // __APPLE__

#endif // APPLE_FUNCTIONS_H_