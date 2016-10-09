# SQLite-service-based-on-ICE
A SQLite service based on ICE (Internet Communications Engine) 

"SQLite is a software library that implements a self-contained, serverless, zero-configuration, transactional SQL database engine". It is widely used in embedded devices. However, its serverless character makes it impossible to be used by remote clients like most RDBMS. In order to solve this problem, we try to make a SQLite server with the help of ICE and CppSQLite. The Internet Communications Engine (ICE) is a comprehensive RPC framework, which makes it very suitable to write server programs; and CppSQLite is a very concise c++ wrapper for SQLite, which makes it easy to handle SQLite.
