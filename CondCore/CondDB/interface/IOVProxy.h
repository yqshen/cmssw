#ifndef CondCore_CondDB_IOVProxy_h
#define CondCore_CondDB_IOVProxy_h
//
// Package:     CondDB
// Class  :     IOVProxy
// 
/**\class IOVProxy IOVProxy.h CondCore/CondDB/interface/IOVProxy.h
   Description: service for read/only access to the condition IOVs.  
*/
//
// Author:      Giacomo Govi
// Created:     Apr 2013
//

#include "CondCore/CondDB/interface/Time.h"
#include "CondCore/CondDB/interface/Types.h"
//
#include <boost/date_time/posix_time/posix_time.hpp>

namespace cond {

  namespace persistency {

    class SessionImpl;
    class IOVProxyData;

    // value semantics. to be used WITHIN the parent session transaction ( therefore the session should be kept alive ).
    class IOVProxy {
    public:
      typedef std::vector<std::tuple<cond::Time_t,cond::Hash> > IOVContainer;
      // more or less compliant with typical iterator semantics...   
      class Iterator  : public std::iterator<std::input_iterator_tag, cond::Iov_t> {
      public:
	//
	Iterator();
	Iterator( IOVContainer::const_iterator current, IOVContainer::const_iterator end, cond::TimeType timeType );
	Iterator( const Iterator& rhs );
	
	//
	Iterator& operator=( const Iterator& rhs );
	
	// returns a VALUE not a reference!
	cond::Iov_t operator*();
	
	//
	Iterator& operator++();
	Iterator operator++(int);
	
	//
	bool operator==( const Iterator& rhs ) const;
	bool operator!=( const Iterator& rhs ) const;
	
      private:
	IOVContainer::const_iterator m_current;
	IOVContainer::const_iterator m_end;
	cond::TimeType m_timeType;
      };
      
    public:
      //
      IOVProxy();
      
      // the only way to construct it from scratch...
      explicit IOVProxy( const std::shared_ptr<SessionImpl>& session );
      
      //
      IOVProxy( const IOVProxy& rhs );
      
      //
      IOVProxy& operator=( const IOVProxy& rhs );
      
      // loads in memory the tag information and the iov groups
      // full=true load the full iovSequence 
      void load( const std::string& tag, bool full=false );
      
      // reset the data in memory and execute again the queries for the current tag 
      void reload();
      
      // clear all the iov data in memory
      void reset();
      
      std::string tag() const; 
      
      cond::TimeType timeType() const;
      
      std::string payloadObjectType() const;
      
      cond::Time_t endOfValidity() const;
      
      cond::Time_t lastValidatedTime() const;
      
      // start the iteration. it referes to the LOADED iov sequence subset, which consists in two consecutive groups - or the entire sequence if it has been requested.
      // returns data only when a find or a load( tag, true ) have been at least called once. 
      Iterator begin() const;
      
      // the real end of the LOADED iov sequence subset.
      Iterator end() const;
      
      // searches the DB for a valid iov containing the specified time.
      // if the available iov sequence subset contains the target time, it does not issue a new query.
      // otherwise, a query according the two consecutives groups is executed.
      Iterator find(cond::Time_t time);
      
      // searches the DB for a valid iov containing the specified time.
      // if the available iov sequence subset contains the target time, it does not issue a new query.
      // otherwise, a query according the two consecutives groups is executed.
      // throws if the target time cannot be found.
      cond::Iov_t getInterval( cond::Time_t time );
      
      // the size of the LOADED iov sequence subset. Matches the real iov size if a load( tag, true ) has been called.
      int size() const;
      
      // for reporting
      size_t numberOfQueries() const;
      
      // for debugging
      std::pair<cond::Time_t,cond::Time_t> loadedGroup() const; 
      
      
    private:
      void checkSession( const std::string& ctx );
      void fetchSequence( cond::Time_t lowerGroup, cond::Time_t higherGroup );
      
    private:
      std::shared_ptr<IOVProxyData> m_data;
      std::shared_ptr<SessionImpl> m_session;
    };
    
  }
}

#endif

