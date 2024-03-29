#include <web3cpp/Utils.h>
#include <web3cpp/Solidity.h>

std::mutex storageLock;

#ifdef __MINGW32__
boost::filesystem::path Utils::GetSpecialFolderPath(int nFolder, bool fCreate) {
  WCHAR pszPath[MAX_PATH] = L"";
  if (SHGetSpecialFolderPathW(nullptr, pszPath, nFolder, fCreate)) {
    return boost::filesystem::path(pszPath);
  }
  return boost::filesystem::path("");
}
#endif

boost::filesystem::path Utils::getDefaultDataDir() {
  namespace fs = boost::filesystem;
  #ifdef __MINGW32__
    return GetSpecialFolderPath(CSIDL_APPDATA) / "web3cpp";
  #else
    fs::path pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == NULL || strlen(pszHome) == 0)
      pathRet = fs::path("/");
    else
      pathRet = fs::path(pszHome);
  #ifdef __APPLE__
    return pathRet / "Library/Application Support/web3cpp";
  #else
    return pathRet / ".web3cpp";
  #endif
  #endif
}

uint64_t Utils::roundUp(uint64_t num, uint64_t mul) {
  return (mul != 0 && num % mul != 0) ? (num + mul - (num % mul)) : num;
}

std::string Utils::randomHex(unsigned int size, bool prefixed) {
  unsigned char saltBytes[size];
  RAND_bytes(saltBytes, sizeof(saltBytes));
  std::string ret = (prefixed) ? "0x" : "";
  ret += dev::toHex(
    dev::sha3(std::string((char*)saltBytes, sizeof(saltBytes)), false)
  ).substr(0, size * 2);
  return ret;
}

BigNumber Utils::toBN(const std::string& number) {
  if (isHexStrict(number)) {
    return hexToBigNumber(number);
  } else if (isNumber(number)) {
    return boost::lexical_cast<BigNumber>(number);
  }
  throw std::invalid_argument("invalid number value (not hex or uint string)");
}

std::string Utils::sha3(const std::string& string, bool isNibble) {
  return (!string.empty()) ? dev::toHex(dev::sha3(string, isNibble)) : "";
}

std::string Utils::keccak256(const std::string& string, bool isNibble) {
  return Utils::sha3(string, isNibble);
}

std::string Utils::sha3Raw(const std::string& string, bool isNibble) {
  return dev::toHex(dev::sha3(string, isNibble));
}

std::string Utils::soliditySha3(json params, Error &err) {
  std::string ret = Utils::stripHexPrefix(Solidity::packMulti(params, err));
  return ((err.getCode() == 0) ? "0x" + Utils::sha3(ret, true) : "");
}

std::string Utils::soliditySha3Raw(json params, Error &err) {
  std::string ret = Utils::stripHexPrefix(Solidity::packMulti(params, err));
  return ((err.getCode() == 0) ? "0x" + Utils::sha3Raw(ret, true) : "");
}

bool Utils::isHex(const std::string& hex) {
  if (hex.substr(0, 2) == "0x" || hex.substr(0, 2) == "0X") { 
    return (hex.substr(2).find_first_not_of("0123456789abcdefABCDEF") == std::string::npos); 
  }
  return (hex.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos);
}

bool Utils::isHexStrict(const std::string& hex) {
  return (hex.substr(0, 2) == "0x" || hex.substr(0, 2) == "0X") ? isHex(hex) : false;
}

bool Utils::isNumber(const std::string& str) {
  return (str.find_first_not_of("0123456789") == std::string::npos);
}

bool Utils::isAddress(const std::string& address) {
  // Regexes for checking the basic requirements of an address,
  // all lower or all upper case, respectively.
  std::regex addRegex = std::regex("^(0x|0X)?[0-9a-fA-F]{40}$");
  std::regex lowRegex = std::regex("^(0x|0X)?[0-9a-f]{40}$");
  std::regex uppRegex = std::regex("^(0x|0X)?[0-9A-F]{40}$");
  if (!std::regex_match(address, addRegex)) {
    return false;
  } else if (std::regex_match(address, lowRegex) || std::regex_match(address, uppRegex)) {
    return true;
  } else {
    return checkAddressChecksum(address);
  }
}

std::string Utils::toLowercaseAddress(const std::string& address) {
  std::string ret = address;
  if (ret.substr(0, 2) == "0x" || ret.substr(0, 2) == "0X") {
    ret = ret.substr(2);
  }
  std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return "0x" + ret;
}

std::string Utils::toUppercaseAddress(const std::string& address) {
  std::string ret = address;
  if (ret.substr(0, 2) == "0x" || ret.substr(0, 2) == "0X") {
    ret = ret.substr(2);
  }
  std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
  return "0x" + ret;
}

std::string Utils::toChecksumAddress(std::string address) {
  // Hash needs address to be all lower-case and without the "0x" part
  address = Utils::toLowercaseAddress(address);
  if (address.substr(0, 2) == "0x" || address.substr(0, 2) == "0X") {
    address = address.substr(2);
  }
  std::string hash = dev::toHex(dev::sha3(address));
  std::string ret;
  for (int i = 0; i < address.length(); i++) {
    if (std::isdigit(address[i])) {
      // [0-9] - Can't uppercase so we skip it
      ret += address[i];
    } else {
      // [A-F] - If character hash is 8-F then make it uppercase
      int nibble = std::stoi(hash.substr(i, 1), nullptr, 16);
      ret += (nibble >= 8) ? std::toupper(address[i]) : std::tolower(address[i]);
    }
  }
  return "0x" + ret;
}

bool Utils::checkAddressChecksum(std::string address) {
  // Hash needs address to be all lower-case and without the "0x" part
  if (address.substr(0, 2) == "0x" || address.substr(0, 2) == "0X") {
    address = address.substr(2);
  }
  std::string hash = dev::toHex(dev::sha3(
    Utils::toLowercaseAddress(address).substr(2)
  ));
  for (int i = 0; i < address.length(); i++) {
    if (!std::isdigit(address[i])) {  // Only check A-F
      int nibble = std::stoi(hash.substr(i, 1), nullptr, 16);
      if (
        (nibble >= 8 && !std::isupper(address[i])) ||
        (nibble < 8 && !std::islower(address[i]))
      ) {
        return false;
      }
    }
  }
  return true;
}

std::string Utils::toHex(const std::string& value) {
  std::stringstream ss;
  if (std::all_of(value.begin(), value.end(), ::isdigit)) { // Number string
    return toHex(boost::lexical_cast<dev::u256>(value));
  } else {  // Text string
    for (int i = 0; i < value.length(); i++) {
      ss << std::hex << (int)value[i];
    }
  }
  return ss.str();
}

std::string Utils::toHex(BigNumber value) {
  std::stringstream ss;
  ss << std::hex << value;
  return ss.str();
}

std::string Utils::stripHexPrefix(const std::string& str) {
  return (
    !str.empty() && isHex(str) && (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X")
  ) ? str.substr(2) : str;
}

std::string Utils::hexToNumberString(const std::string& hex) {
  return boost::lexical_cast<std::string>(toBN(hex));
}

BigNumber Utils::hexToBigNumber(std::string hex) {
  if (hex.substr(0,2) == "0x" || hex.substr(0, 2) == "0X") { hex = hex.substr(2); }
  return boost::lexical_cast<HexTo<dev::u256>>(hex);
}

std::string Utils::hexToUtf8(std::string hex) {
  if (!isHexStrict(hex)) {
    throw std::string("Error converting non-hex value to UTF-8: ") + hex;
  }
  hex = hex.substr(2);  // Remove "0x"

  std::string str;
  for (int i = 0; i < hex.length(); i += 2) {
    str += std::stoul(hex.substr(i, 2), nullptr, 16);
  }

  return str;
}

std::string Utils::hexToString(const std::string& hex) {
  return Utils::hexToUtf8(hex);
}

std::string Utils::hexToAscii(std::string hex) {
  if (!isHexStrict(hex)) {
    throw std::string("Error converting non-hex value to ASCII: ") + hex;
  }
  hex = hex.substr(2);  // Remove "0x"

  std::string str;
  for (int i = 0; i < hex.length(); i += 2) {
    str += std::stoul(hex.substr(i, 2), nullptr, 16);
  }
  return str;
}

std::vector<unsigned int> Utils::hexToBytes(std::string hex) {
  if (!isHexStrict(hex)) {
    throw std::string("Error converting non-hex value to bytes: ") + hex;
  }
  hex = hex.substr(2);  // Remove "0x"

  std::vector<unsigned int> bytes;
  for (int i = 0; i < hex.length(); i += 2) {
    bytes.push_back(std::stoul(hex.substr(i, 2), nullptr, 16));
  }
  return bytes;
}

std::string Utils::utf8ToHex(const std::string& str) {
  std::string hex = "";

  std::stringstream ss;
  for (int i = 0; i < str.length(); i++) {
    // You need two casts in order to properly cast char to uint.
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint>(static_cast<uint8_t>(str[i]));
  }
  hex += ss.str();

  return "0x" + hex;
}

std::string Utils::stringToHex(const std::string& hex) {
  return Utils::utf8ToHex(hex);
}

std::string Utils::asciiToHex(const std::string& str) {
  std::string hex = "";

  for (int i = 0; i < str.length(); i++) {
    int code = str[i];
    std::stringstream ss;
    ss << std::hex << code;
    std::string n = ss.str();
    hex += (n.length() < 2) ? "0" + n : n;
  }

  return "0x" + hex;
}

std::string Utils::bytesToHex(const std::vector<unsigned int>& byteArray) {
  std::string hex = "";

  std::ostringstream result;
  result << std::setw(2) << std::setfill('0') << std::hex;
  for (int i = 0; i < byteArray.size(); i++) {
    result << byteArray[i];
  }
  hex = result.str();

  return "0x" + hex;
}

std::string Utils::toWei(const std::string& amount, int decimals) {
  std::string digitPadding = "";
  std::string valuestr = "";

  // Check if input is valid
  if (amount.find_first_not_of("0123456789.") != std::string::npos) return "";

  // Read value from input string
  size_t index = 0;
  while (index < amount.size() && amount[index] != '.') {
    valuestr += amount[index];
    ++index;
  }
  ++index;  // Jump fixed point

  // Check if fixed point exists
  if (amount[index-1] == '.' && (amount.size() - (index)) > decimals) return "";

  // Check if input precision matches digit precision
  if (index < amount.size()) {
    // Read precision point into digitPadding
    while (index < amount.size()) {
      digitPadding += amount[index];
      ++index;
    }
  }

  // Create padding if there are missing decimals
  while (digitPadding.size() < decimals) digitPadding += '0';
  valuestr += digitPadding;
  while (valuestr[0] == '0') valuestr.erase(0,1);
  if (valuestr == "") valuestr = "0";
  return valuestr;
}

std::string Utils::toWei(const BigNumber& amount, int decimals) {
  std::string amountHex = Utils::toHex(amount);
  std::string amountStr = Utils::hexToNumberString(amountHex);
  return toWei(amountStr, decimals);
}

std::string Utils::fromWei(const std::string& amount, int decimals) {
  std::string result;
  if (amount.size() <= decimals) {
    int valueToPoint = decimals - amount.size();
    result += "0.";
    for (int i = 0; i < valueToPoint; ++i) result += "0";
    result += amount;
  } else {
    result = amount;
    int pointToPlace = result.size() - decimals;
    result.insert(pointToPlace, ".");
  }
  if (result == "") result = "0";
  return result;
}

std::string Utils::fromWei(const BigNumber& amount, int decimals) {
  std::string amountHex = Utils::toHex(amount);
  std::string amountStr = Utils::hexToNumberString(amountHex);
  return fromWei(amountStr, decimals);
}

std::string Utils::padLeft(
  std::string string, unsigned int characterAmount, char sign
) {
  bool hasPrefix = (string.substr(0, 2) == "0x" || string.substr(0, 2) == "0X");
  if (hasPrefix) { string = string.substr(2); }
  size_t padding = (characterAmount > string.length())
    ? (characterAmount - string.length()) : 0;
  std::string padded = (padding != 0) ? std::string(padding, sign) : "";
  return (hasPrefix ? "0x" : "") + padded + string;
}

std::string Utils::leftPad(
  std::string string, unsigned int characterAmount, char sign
) {
  return padLeft(string, characterAmount, sign);
}

std::string Utils::padRight(
  std::string string, unsigned int characterAmount, char sign
) {
  bool hasPrefix = (string.substr(0, 2) == "0x" || string.substr(0, 2) == "0X");
  if (hasPrefix) { string = string.substr(2); }
  size_t padding = (characterAmount > string.length())
    ? (characterAmount - string.length()) : 0;
  std::string padded = (padding != 0) ? std::string(padding, sign) : "";
  return (hasPrefix ? "0x" : "") + string + padded;
}

std::string Utils::rightPad(
  std::string string, unsigned int characterAmount, char sign
) {
  return padRight(string, characterAmount, sign);
}

json Utils::readJSONFile(boost::filesystem::path &filePath, Error &err) {
  json ret;
  storageLock.lock();
  if (!boost::filesystem::exists(filePath)) {
    err.setCode(33);  // JSON File Does Not Exist
    return ret;
  }
  try {
    boost::nowide::ifstream jsonFile(filePath.string());
    jsonFile >> ret;
    jsonFile.close();
  } catch (std::exception &e) {
    storageLock.unlock();
    err.setCode(34);  // JSON File Read Error
    return ret;
  }
  storageLock.unlock();
  err.setCode(0);
  return ret;
}

void Utils::writeJSONFile(json &obj, boost::filesystem::path &filePath, Error &err) {
  storageLock.lock();
  try {
    boost::nowide::ofstream os(filePath.string());
    os << std::setw(2) << obj << std::endl;
    os.close();
  } catch (std::exception &e) {
    storageLock.unlock();
    err.setCode(35);  // JSON File Write Error
    return;
  }
  storageLock.unlock();
  err.setCode(0);
}

json Utils::decodeRawTransaction(std::string signedTx) {
  if (signedTx.substr(0, 2) == "0x" || signedTx.substr(0, 2) == "0X") {
    signedTx = signedTx.substr(2); // Remove "0x"
  }

  json ret;
  dev::eth::TransactionBase tx = dev::eth::TransactionBase(
    dev::fromHex(signedTx), dev::eth::CheckTransaction::None
  );

  // Creation, message, sender, receiver and data
  ret["hex"] = tx.sha3().hex();
  if (tx.isCreation()) {
    ret["type"] = "creation";
    ret["code"] = dev::toHex(tx.data());
  } else {
    ret["type"] = "message";
    ret["to"] = boost::lexical_cast<std::string>(tx.to());
    ret["data"] = (tx.data().empty() ? "" : dev::toHex(tx.data()));
  }
  try {
    auto s = tx.sender();
    if (tx.isCreation()) {
      ret["creates"] = boost::lexical_cast<std::string>(dev::toAddress(s, tx.nonce()));
    }
    ret["from"] = boost::lexical_cast<std::string>(s);
  } catch (std::exception &e) {
    ret["from"] = "<unsigned>";
  }

  // Value, nonce, gas limit, gas price, hash and r/s/v signature keys
  ret["value"] = Utils::fromWei(boost::lexical_cast<std::string>(tx.value()), 18) + " AVAX";
  ret["nonce"] = boost::lexical_cast<std::string>(tx.nonce());
  ret["gas"] = boost::lexical_cast<std::string>(tx.gas());
  ret["price"] = dev::eth::formatBalance(tx.gasPrice()) + " (" +
    boost::lexical_cast<std::string>(tx.gasPrice()) + " wei)";
  ret["hash"] = tx.sha3(dev::eth::WithoutSignature).hex();
  if (tx.safeSender()) {
    ret["r"] = boost::lexical_cast<std::string>(tx.signature().r);
    ret["s"] = boost::lexical_cast<std::string>(tx.signature().s);
    ret["v"] = boost::lexical_cast<std::string>(tx.signature().v);
  }

  // Timestamps (epoch and human-readable) and confirmed
  const auto p1 = std::chrono::system_clock::now();
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::stringstream timestream;
  timestream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
  ret["humanDate"] = timestream.str();
  ret["confirmed"] = false;
  ret["unixDate"] = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
  ret["invalid"] = false;
  ret["signature"] = signedTx;

  return ret;
}

