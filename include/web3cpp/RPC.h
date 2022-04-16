#ifndef RPC_H
#define RPC_H

#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>

#include <web3cpp/Error.h>
#include <web3cpp/Utils.h>

// Module that helps with building JSON-RPC requests, for use in Eth.
// Every function returns a formatted and ready-to-use JSON object that
// can be sent through an HTTP request to the network.
// When required, functions will do sanity checks on inputs and will set
// error codes and return empty JSON objects if those checks fail.
// Every JSON object returned defaults to an id of "1".
// See the docs at https://eth.wiki/json-rpc/API

// TODO: streamline sanity checks into their own functions

namespace RPC {
  json _buildJSON(std::string method, json params = json::array());

  json web3_clientVersion();
  json web3_sha3(std::string data, Error &err);

  json net_version();
  json net_listening();
  json net_peerCount();

  json eth_protocolVersion();
  json eth_syncing();
  json eth_coinbase();
  json eth_mining();
  json eth_hashrate();
  json eth_gasPrice();
  json eth_accounts();
  json eth_blockNumber();
  json eth_getBalance(std::string address, std::string defaultBlock, Error &err);
  json eth_getBalance(std::string address, BigNumber defaultBlock, Error &err);
  json eth_getStorageAt(std::string address, std::string position, std::string defaultBlock, Error &err);
  json eth_getStorageAt(std::string address, std::string position, BigNumber defaultBlock, Error &err);
  json eth_getTransactionCount(std::string address, std::string defaultBlock, Error &err);
  json eth_getTransactionCount(std::string address, BigNumber defaultBlock, Error &err);
  json eth_getBlockTransactionCountByHash(std::string hash, Error &err);
  json eth_getBlockTransactionCountByNumber(std::string number);
  json eth_getBlockTransactionCountByNumber(BigNumber number);
  json eth_getUncleCountByBlockHash(std::string hash, Error &err);
  json eth_getUncleCountByBlockNumber(std::string number);
  json eth_getUncleCountByBlockNumber(BigNumber number);
  json eth_getCode(std::string address, std::string defaultBlock, Error &err);
  json eth_getCode(std::string address, BigNumber defaultBlock, Error &err);
  json eth_sign(std::string address, std::string data, Error &err);
  json eth_signTransaction(json transactionObject);
  json eth_sendTransaction(json transactionObject);
  json eth_sendRawTransaction(std::string signedTxData, Error &err);
  json eth_call(json callObject, std::string defaultBlock, Error &err);
  json eth_call(json callObject, BigNumber defaultBlock, Error &err);
  json eth_estimateGas(json callObject);
  json eth_getBlockByHash(std::string hash, bool returnTransactionObjects, Error &err);
  json eth_getBlockByNumber(std::string number, bool returnTransactionObjects);
  json eth_getBlockByNumber(BigNumber number, bool returnTransactionObjects);
  json eth_getTransactionByHash(std::string hash, Error &err);
  json eth_getTransactionByBlockHashAndIndex(std::string hash, std::string index, Error &err);
  json eth_getTransactionByBlockNumberAndIndex(std::string number, std::string index, Error &err);
  json eth_getTransactionByBlockNumberAndIndex(BigNumber number, std::string index, Error &err);
  json eth_getTransactionReceipt(std::string hash, Error &err);
  json eth_getUncleByBlockHashAndIndex(std::string hash, std::string index, Error &err);
  json eth_getUncleByBlockNumberAndIndex(std::string number, std::string index, Error &err);
  json eth_getUncleByBlockNumberAndIndex(BigNumber number, std::string index, Error &err);
  json eth_getCompilers();
  json eth_compileSolidity(std::string sourceCode);
  json eth_compileLLL(std::string sourceCode);
  json eth_compileSerpent(std::string sourceCode);
  json eth_newFilter(json filterOptions);
  json eth_newBlockFilter();
  json eth_newPendingTransactionFilter();
  json eth_uninstallFilter(std::string filterId, Error &err);
  json eth_getFilterChanges(std::string filterId, Error &err);
  json eth_getFilterLogs(std::string filterId, Error &err);
  json eth_getLogs(json filterOptions);
  json eth_getWork();
  json eth_submitWork(std::string nonce, std::string powHash, std::string digest, Error &err);
  json eth_submitHashrate(std::string hashrate, std::string id, Error &err);

  json db_putString(std::string dbName, std::string key, std::string value);
  json db_getString(std::string dbName, std::string key);
  json db_putHex(std::string dbName, std::string key, std::string value, Error &err);
  json db_getHex(std::string dbName, std::string key);

  json shh_version();
  json shh_post(json whisperPostObject);
  json shh_newIdentity();
  json shh_hasIdentity();
  json shh_newGroup();
  json shh_addToGroup(std::string identityAddress, Error &err);
  json shh_newFilter(json filterOptions);
  json shh_uninstallFilter(std::string filterId, Error &err);
  json shh_getFilterChanges(std::string filterId, Error &err);
  json shh_getMessages(std::string filterId, Error &err);
};

#endif  // RPC_H
