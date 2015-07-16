# SQLite-service-based-on-ICE
A SQLite service based on ICE(Internet Communications Engine)

"SQLite is a software library that implements a self-contained, serverless, zero-configuration, transactional SQL database engine". It was widely used incluing in our embedded devices.
However, its serverless character make it impossible to used by remote clients like most RDBMS. In order to solve this problem, we try to make a SQLite server by the help of ICE and CppSQLite. 
The Internet Communications Engine (ICE) is a comprehensive RPC framework, which make it very suitable to write server programs; and CppSQLite is a very concise c++ wrapper for SQLite, which make it easy to nandle SQLite.
