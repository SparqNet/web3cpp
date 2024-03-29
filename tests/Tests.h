#ifndef TESTS_H
#define TESTS_H

#include <bitset>
#include <chrono>
#include <ctime>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <web3cpp/Web3.h>
#include <web3cpp/Contract.h>
#include <web3cpp/Solidity.h>
#include <web3cpp/Utils.h>

// Helper class for unit testing.

class Tests {
  private:
    // Password for the wallet and log stream object for writing test processes
    std::string password;
    std::ofstream logStream;

    // Log file and test wallet folder
    std::string logFile = "web3cpp-test-log.txt";
    boost::filesystem::path walletFolder = "web3cpp-test-wallet";

    // Counters for passed/failed tests, error reason (if any),
    // and functions for controlling those
    int pass = 0;
    int fail = 0;
    bool curPass;
    std::string curReason;
    void passed() { curPass = true; curReason = ""; pass++; }
    void failed(std::string msg) { curPass = false; curReason = msg; fail++; }

  public:
    // The Web3 object that will be tested
    Web3* web3 = NULL;

    // Constructor
    Tests(std::string &password) {
      web3 = new Web3(Provider("avax-c-test"), walletFolder);
      Error error;
      if (!web3->wallet.loadWallet(password, error)) {
        std::cout << std::string("Could not load wallet: ") << error.what() << std::endl;
        throw;
      }
      this->password = password;
      logStream.open(logFile, std::ofstream::out | std::ofstream::app);
      std::time_t t = std::time(nullptr);
      std::tm* tm = std::localtime(&t);
      std::stringstream timestream;
      timestream << std::put_time(tm, "%d-%m-%Y %H-%M-%S");
      logStream << "[web3cpp Test Log - " << timestream.str() << "]\n\n";
    }

    // Destructor
    ~Tests() { logStream.close(); delete web3; }

    // Show overall test results
    void showResults() {
      std::cout <<
        "* RESULTS: " << pass << " passed, " << fail << " failed" << std::endl
        << "* Check the log file " << logFile << " for details"
      << std::endl;
    }

    /**
     * Tests start here, divided by class.
     * Each test should be completely contained into a function of its own
     * and output the details to the log file.
     */

    // ==============================================================
    // BIP39
    // ==============================================================

    // Test for generating a new mnemonic.
    void testMnemonicGeneration();

    // Test for generating a key pair based on a mnemonic and derivation path.
    void testKeyPairGeneration();

    // Test for checking valid words for mnemonics.
    void testMnemonicWordValidity();

    // Test for generating addresses from a given seed, derivation path and index.
    void testMnemonicAddressGeneration();

    // ==============================================================
    // DATABASE
    // ==============================================================

    // Test for all Database operations at once.
    void testDatabaseCRUD();

    // ==============================================================
    // NET
    // ==============================================================

    // Tests for normal and custom HTTP requests, respectively.
    void testHTTPRequest();
    void testCustomHTTPRequest();

    // ==============================================================
    // CIPHER
    // ==============================================================

    // Tests for encrypting and decrypting data (normal and legacy).
    void testCipherNormal();
    void testCipherLegacy();

    // ==============================================================
    // SOLIDITY
    // ==============================================================

    // Test for checking the validity of each supported type.
    void testTypeChecks();

    // Tests for packing each supported type individually.
    void testFunction();
    void testUint();
    void testAddress();
    void testBool();
    void testBytes();
    void testString();
    void testUintArray();
    void testAddressArray();
    void testBoolArray();
    void testBytesArray();
    void testStringArray();

    // Test for encoding a complete ABI.
    void testMulti();

    // ==============================================================
    // UTILS
    // ==============================================================
    // Test for random hex string generation at preset sizes.
bool testRandomHexes() {
  std::vector<unsigned int> sizes = {4, 8, 16, 32};
  std::vector<std::string> results;
  std::vector<std::string> resultsPrefixed;
  bool allPass = true;

  for (unsigned int size : sizes) {
    std::string res = Utils::randomHex(size);
    std::string resPfx = Utils::randomHex(size, true);
    results.push_back(res);
    resultsPrefixed.push_back(resPfx);
    if (allPass &&
      ((res.length() / 2) != size || ((resPfx.length() - 2) / 2) != size)
    ) {
      allPass = false;
    }
  }

  if (!allPass) {
    return false;
  } else {
    return true;
  }
}

//Test bigNumber conversion
bool testBigNumbers() {
  std::string decNumber = "1234567890987654321";
  std::string hexNumber = "0x112210f4b16c1cb1";

  BigNumber decBN = Utils::toBN(decNumber);
  BigNumber hexBN = Utils::toBN(hexNumber);
  std::string decBNStr = boost::lexical_cast<std::string>(decBN);
  std::string hexBNStr = "0x" + Utils::toHex(hexBN);

  if (decBN != hexBN) {
    return false;
  } else if (decBNStr != decNumber) {
    return false;
  } else if (hexBNStr != hexNumber) {
    return false;
  } else {
    return true;
  }
}



    // Test for SHA3 hashing (normal and raw).
 bool testSHA3() {
  std::string strA = "Test String";
  std::string strB = "";  // Empty on purpose, for testing sha3Raw()
  std::string strHash = "5a3f690ae57a7ead22e6ced03d1c290406abe7adffcd556d84244ed91b119ee8";
  std::string zeroHash = "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";

  std::string hashA1 = Utils::sha3(strA);
  std::string hashA2 = Utils::sha3Raw(strA);
  std::string hashB1 = Utils::sha3(strB);
  std::string hashB2 = Utils::sha3Raw(strB);

  if (
    (hashA1 != strHash) || (hashA2 != strHash) ||
    (hashB1 != "") || (hashB2 != zeroHash)
  ) {
    return false;
  } else {
    return true;
  }
}

    // Test for Solidity SHA3 hashing (normal only).
bool testSoliditySHA3() {
  json j1A = { {"type", "uint256"}, {"value", "129831751235123"} };
  json j1B = { {"type", "address"}, {"value", "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8"} };
  json j1C = { {"type", "bool"}, {"value", "true"} };
  json j1D = { {"type", "bytes"}, {"value", "0xaaaa"} };
  json j1E = { {"type", "string"}, {"value", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque eget porttitor tortor, et tincidunt nibh. Aenean erat quam, maximus id gravida sit amet, rhoncus sed nulla. Curabitur maximus tellus diam, vel vulputate sapien maximus vitae. Duis consectetur, felis at efficitur consectetur, tortor nisl bibendum mauris, eget venenatis augue justo et orciSimple "} };
  json j2A = { {"type", "uint256[]"}, {"value", {
    "56982562956398", "32456239756329", "432985637983"
  }}};
  json j2B = { {"type", "address[]"}, {"value", {
    "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8",
    "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8",
    "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8"
  }}};
  json j2C = { {"type", "bool[]"}, {"value", {
    "true", "false", "0", "1"
  }}};
  json j2D = { {"type", "bytes[]"}, {"value", {
    "0xaaa",
    "0xbbb",
    "0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  }}};
  json j2E = { {"type", "string[]"}, {"value", {
    "aaaaa",
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
    "ccc"
  }}};

  std::string r1A = "0x23ca82fd08978ec8bdedfb49a8c82cc8bd1f2f1e6734f39e7d54bfe13dc36c25";
  std::string r1B = "0x512f675e26efc88a1dba5491171ed07ca7bdec44d0822a65eeeb06b3f8c636d0";
  std::string r1C = "0xb10e2d527612073b26eecdfd717e6a320cf44b4afac2b0732d9fcbe2b7fa0cf6";
  std::string r1D = "0x785afe2f11b7ad91f27a14f605ff28694ce75d51df9fa82d5a178448893ee6cb";
  std::string r1E = "0x5be763b4d237e94da1a94efdeb0d27d702ec7d90d7226307c3961dc9168796dd";
  std::string r2A = "0x2b3bdb7963d9b3933a75696003efcfd204e9fed6ad8ac5fb5f1d0fc30f581e14";
  std::string r2B = "0xce8b8dd445113f4f9c65f28b9b7c283c4a9e509210b167af48bb84afa92b90e7";
  std::string r2C = "0xf546bb86c6f0591cc041d7b8f9edfc99f3e78d2bee777276081f2061d5970909";
  std::string r2D = "0x0dc9e533a18a0ab469b42a579e67be5d6753a2a22cecc68fc383ff90d40ba15e";
  std::string r2E = "0xaf4852f412ee1759755a9ab487583bc13ba55368dcee85053dadc7b1a62e9a8c";

  Error e1A, e1B, e1C, e1D, e1E, e2A, e2B, e2C, e2D, e2E;
  std::string h1A = Utils::soliditySha3(j1A, e1A);
  std::string h1B = Utils::soliditySha3(j1B, e1B);
  std::string h1C = Utils::soliditySha3(j1C, e1C);
  std::string h1D = Utils::soliditySha3(j1D, e1D);
  std::string h1E = Utils::soliditySha3(j1E, e1E);
  std::string h2A = Utils::soliditySha3(j2A, e2A);
  std::string h2B = Utils::soliditySha3(j2B, e2B);
  std::string h2C = Utils::soliditySha3(j2C, e2C);
  std::string h2D = Utils::soliditySha3(j2D, e2D);
  std::string h2E = Utils::soliditySha3(j2E, e2E);

  bool errOk = true;
  if (e1A.getCode() != 0) { errOk = false; h1A = e1A.what(); }
  if (e1B.getCode() != 0) { errOk = false; h1B = e1B.what(); }
  if (e1C.getCode() != 0) { errOk = false; h1C = e1C.what(); }
  if (e1D.getCode() != 0) { errOk = false; h1D = e1D.what(); }
  if (e1E.getCode() != 0) { errOk = false; h1E = e1E.what(); }
  if (e2A.getCode() != 0) { errOk = false; h2A = e2A.what(); }
  if (e2B.getCode() != 0) { errOk = false; h2B = e2B.what(); }
  if (e2C.getCode() != 0) { errOk = false; h2C = e2C.what(); }
  if (e2D.getCode() != 0) { errOk = false; h2D = e2D.what(); }
  if (e2E.getCode() != 0) { errOk = false; h2E = e2E.what(); }
  if (!errOk) {
    return false;
  } else {
    int passTotal = 10;
    int passCount = (
      (h1A == r1A) + (h1B == r1B) + (h1C == r1C) + (h1D == r1D) + (h1E == r1E) +
      (h2A == r2A) + (h2B == r2B) + (h2C == r2C) + (h2D == r2D) + (h2E == r2E)
    );
    if (passCount != passTotal) {
      return false;
    } else {
      return true;
    }
  }
}


  // Test for Solidity SHA3 hashing (raw only).
bool testSoliditySHA3Raw() {
  json jZero = { {"type", "string"}, {"value", ""} };
  std::string zeroHash = "0x5380ea0b298eb1aa6da7e38c8e5d4fa6839b22f7d6bf259874a6cf7d25cce827";

  Error e;
  std::string zeroHashRes = Utils::soliditySha3Raw(jZero, e);
  if (e.getCode() != 0) {
    zeroHashRes = e.what();
    return false;
  } else if (zeroHashRes != zeroHash) {
    return false;
  } else {
    return true;
  }
}


    // Test for hex string detection.
bool testHexes() {
  std::vector<std::string> hexStrings = {
    "0x0a1B2c3D4e5F6789", "0x0g1H2i3J4k5L6789",
    "0a1B2c3D4e5F6789", "0g1H2i3J4k5L6789"
  };
  std::vector<std::pair<bool, bool>> hexPassExpect = {
    {true, true}, {false, false},
    {true, false}, {false, false}
  };
  std::vector<std::pair<bool, bool>> hexPassResult;

  for (std::string hex : hexStrings) {
    hexPassResult.push_back({Utils::isHex(hex), Utils::isHexStrict(hex)});
  }

  bool allPass = true;
  for (int i = 0; i < hexPassExpect.size(); i++) {
    if (allPass &&
      (hexPassResult[i].first != hexPassExpect[i].first) ||
      (hexPassResult[i].second != hexPassExpect[i].second)
    ) {
      allPass = false; break;
    }
  }
  if (!allPass) {
   return false;
  } else {
    return true;
  }
}


 // Test for address detection.
bool testAddresses() {
  std::vector<std::string> addresses = {
    "0x3E8467983bA80734654208b274EBf01264526117",
    "0x3e8467983ba80734654208b274ebf01264526117",
    "0X3E8467983BA80734654208B274EBF01264526117",
    "0x3e8467983Ba80734654208B274ebF01264526117",
    "0x3e8467983Ba80734654208B274ebF0126452611"
  };
  std::vector<bool> expects = {true, true, true, false, false};
  std::vector<bool> results;

  for (std::string address : addresses) {
    results.push_back(Utils::isAddress(address));
  }

  bool allPass = true;
  for (int i = 0; i < expects.size(); i++) {
    if (allPass && results[i] != expects[i]) {
      allPass = false; break;
    }
  }
  if (!allPass) {
    return false;
  } else {
    return true;
  }
}

     // Test for address conversions (upper, lower and checksum).
bool testAddressConversions() {
  std::string lowerAdd = "0x3e8467983ba80734654208b274ebf01264526117";
  std::string upperAdd = "0x3E8467983BA80734654208B274EBF01264526117";
  std::string checkAdd = "0x3E8467983bA80734654208b274EBf01264526117";

  std::string lowerRes = Utils::toLowercaseAddress(upperAdd);
  std::string upperRes = Utils::toUppercaseAddress(lowerAdd);
  std::string checkRes = Utils::toChecksumAddress(lowerAdd);

  if (lowerRes != lowerAdd || upperRes != upperAdd || checkRes != checkAdd) {
    return false;
  } else {
    return true;
  }
}



    // Test for address checksums
bool testAddressChecksums() {
  std::vector<std::string> addresses = {
    "0x3E8467983bA80734654208b274EBf01264526117",
    "0x3e8467983Ba80734654208B274ebF01264526117",
    "0x3e8467983ba80734654208b274ebf01264526117",
    "0x3E8467983BA80734654208B274EBF01264526117"
  };
  std::vector<bool> expects = {true, false, false, false};
  std::vector<bool> results;

  for (std::string address : addresses) {
    results.push_back(Utils::checkAddressChecksum(address));
  }

  bool allPass = true;
  for (int i = 0; i < expects.size(); i++) {
    if (results[i] != expects[i]) { allPass = false;
    break;}
  }
  if (allPass == true) {
    return true;
  } else {
    return false;
  }
}



    // Test for hex data conversions (number, number string and text string).
bool testHexConversions() {
  BigNumber bn(836502639245);
  std::string bnStr = "836502639245";
  std::string textStr = "Test String";
  std::string bnHex = "c2c371528d";
  std::string textHex = "5465737420537472696e67";

  std::string bnRes = Utils::toHex(bn);
  std::string bnStrRes = Utils::toHex(bnStr);
  std::string textRes = Utils::toHex(textStr);

  if ((bnRes != bnHex) || (bnStrRes != bnHex) || (textRes != textHex)) {
    return false;
  } else {
    return true;
  }
}


// Test for hex prefix stripping.
 bool testHexStripping() {
  std::vector<std::string> hexes = {"0x12345", "0X12345", "0xghijk", ""}; // Last one is empty on purpose
  std::vector<std::string> expects = {"12345", "12345", "0xghijk", ""};
  std::vector<std::string> results;

  for (std::string hex : hexes) {
    results.push_back(Utils::stripHexPrefix(hex));
  }

  bool allPass = true;
  for (int i = 0; i < expects.size(); i++) {
    if (allPass && results[i] != expects[i]) {
      allPass = false; break;
    }
  }
  if (!allPass) {
    return false;
  } else {
    return true;
  }
}

// Test for type conversions, from HEX to something.
bool testFromHexToTypes() {
  std::string bnHex = "0x16a5b628c2de92f4";
  std::string utf8Hex = "0xe6bca2e5ad97";
  std::string asciiHex = "0x5465737420537472696e67";
  std::string bytesHex = "0x5465737420537472696e67";
  BigNumber bnExp(1631910726175986420);
  std::string bnStrExp = "1631910726175986420";
  std::string utf8Exp = u8"漢字";
  std::string asciiExp = "Test String";
  std::vector<unsigned int> bytesExp = {
    84, 101, 115, 116, 32, 83, 116, 114, 105, 110, 103
  };

  BigNumber bnRes(Utils::hexToBigNumber(bnHex));
  std::string bnStrRes = Utils::hexToNumberString(bnHex);
  std::string utf8Res = Utils::hexToUtf8(utf8Hex);
  std::string asciiRes = Utils::hexToAscii(asciiHex);
  std::vector<unsigned int> bytesRes = Utils::hexToBytes(bytesHex);

  if (
    (bnRes != bnExp) || (bnStrRes != bnStrExp) ||
    (utf8Res != utf8Exp) || (asciiRes != asciiExp) || (bytesRes != bytesExp)
  ) {
    return false;
  } else {
    return true;
  }
}

    
    

    // Test for type conversions, from something to HEX.
bool testFromTypesToHex() {
  std::string utf8Str = u8"漢字";
  std::string asciiStr = "Test String";
  std::vector<unsigned int> bytesStr = {
    84, 101, 115, 116, 32, 83, 116, 114, 105, 110, 103
  };

  std::string utf8Exp = "0xe6bca2e5ad97";
  std::string asciiExp = "0x5465737420537472696e67";
  std::string bytesExp = "0x5465737420537472696e67";

  std::string utf8Res = Utils::utf8ToHex(utf8Str);
  std::string asciiRes = Utils::asciiToHex(asciiStr);
  std::string bytesRes = Utils::bytesToHex(bytesStr);

  if ((utf8Res != utf8Exp) || (asciiRes != asciiExp) || (bytesRes != bytesExp)) {
    return false;
  } else {
    return true;
  }
}


    // Test for currency conversions, from Wei to fixed point and vice-versa.
bool testWeiConversions() {
  std::vector<std::pair<std::string, int>> amountsTo = {
    {"15", 18}, {"30", 9}, {"5", 2}, {"2", 5}
  };
  std::vector<std::pair<std::string, int>> amountsFrom = {
    {"5000000000000000000", 18}, {"1000000000", 9}, {"80000", 4}, {"1337", 2}
  };
  std::vector<std::string> toExp = {
    "15000000000000000000", "30000000000", "500", "200000"
  };
  std::vector<std::string> fromExp = {
    "5.000000000000000000", "1.000000000", "8.0000", "13.37"
  };
  std::vector<std::string> toRes, fromRes;

  for (int i = 0; i < amountsTo.size(); i++) {
    toRes.push_back(Utils::toWei(amountsTo[i].first, amountsTo[i].second));
  }
  for (int i = 0; i < amountsFrom.size(); i++) {
    fromRes.push_back(Utils::fromWei(amountsFrom[i].first, amountsFrom[i].second));
  }

  bool allPass = true;
  for (int i = 0; i < toExp.size(); i++) {
    if (allPass && toExp[i] != toRes[i]) { allPass = false; break; }
  }
  for (int i = 0; i < fromExp.size(); i++) {
    if (allPass && fromExp[i] != fromRes[i]) { allPass = false; break; }
  }
  if (!allPass) {
    return false;
  } else {
  return true;
  }
}

// Test for HEX string padding (left and right).
bool testHexPadding() {
  std::string hexRaw = "DeaDBeeF";
  std::string hexPfx = "0xDeaDBeeF";
  std::vector<int> sizes = {10, 12, 14, 16};
  std::vector<std::pair<std::string, std::string>> rawPads;
  std::vector<std::pair<std::string, std::string>> pfxPads;
  bool allPass = true;

  for (int size : sizes) {
    rawPads.push_back({Utils::padLeft(hexRaw, size), Utils::padRight(hexRaw, size)});
    pfxPads.push_back({Utils::padLeft(hexPfx, size), Utils::padRight(hexPfx, size)});
  }

  for (int i = 0; i < sizes.size(); i++) {
    if (allPass && (
      rawPads[i].first.length() != sizes[i] ||
      rawPads[i].second.length() != sizes[i] ||
      pfxPads[i].first.length() != sizes[i] + 2 ||
      pfxPads[i].second.length() != sizes[i] + 2
    )) {
      allPass = false; break;
    }
  }
  if (!allPass) {
    return false;
  } else {
   return true;
  }
}

   

    // ==============================================================
    // WALLET
    // ==============================================================

    /**
     * Test for valid Account generation.
     * Created test Account is automatically deleted at the end of the test.
     */
    void generateAccount();

    // Tests for password check and storage/clearing, respectively.
    void handlePasswordCheck();
    void handlePasswordStorageAuto();
    void handlePasswordStorageManual();

    /**
     * Test for signing and recovering a signed message.
     * Require at least ONE existing Account, which is created automatically
     * if it doesn't exist. Test WILL fail if this part fails too.
     */
    void signAndRecoverMessage();

    /**
     * Test for building, signing, sending and storing a transaction in history.
     * Require at least ONE existing Account WITH FUNDS.
     * Account will NOT be automatically created and funds can NOT be
     * automatically fetched.
     * Make sure you go to a faucet and get AT LEAST 0.01 AVAX
     * before running the test, otherwise it'll automatically fail.
     */
    void testTransaction();

    // ==============================================================
    // CONTRACT
    // ==============================================================

    // Test for proper Contract loading and ABI manipulation.
    // "Legacy" tests the older method, "Normal" tests the newer one.
    // Check Contract.h for more details.



};

#endif // TESTS_H
