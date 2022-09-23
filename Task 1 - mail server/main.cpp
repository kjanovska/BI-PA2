#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>

#endif /* __PROGTEST */
using namespace std;

#ifndef __PROGTEST__

class CTimeStamp {
public:
    CTimeStamp(int year,
               int month,
               int day,
               int hour,
               int minute,
               int sec);

    int Compare(const CTimeStamp &x) const;

    friend ostream &operator<<(ostream &os,
                               const CTimeStamp &x);

private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;
};

CTimeStamp::CTimeStamp(int year, int month, int day, int hour, int minute, int sec) {
    this->year = year;
    this->month = month;
    this->day = day;
    this->hour = hour;
    this->minute = minute;
    this->sec = sec;
}

int CTimeStamp::Compare(const CTimeStamp &x) const {
    // -1 pokud je this mensi nez x
    if (this -> year > x.year) return 1;
    if (this -> year < x.year) return -1;

    if (this -> month > x.month) return 1;
    if (this -> month < x.month) return -1;

    if (this -> day > x.day) return 1;
    if (this -> day < x.day) return -1;

    if (this -> hour > x.hour) return 1;
    if (this -> hour < x.hour) return -1;

    if (this -> minute > x.minute) return 1;
    if (this -> minute < x.minute) return -1;

    if (this -> sec > x.sec) return 1;
    if (this -> sec < x.sec) return -1;

    return 0;
}

ostream &operator<<(ostream &os, const CTimeStamp &x) {
    //"2014-03-31 13:26:23 "
    os << x.year << "-";
    if ( x.month <= 9 ) os << "0" << x.month << "-";
    else os << x.month << "-";

    if ( x.day <= 9 ) os << "0" << x.day << " ";
    else os << x.day << " ";

    if ( x.hour <= 9 ) os << "0" << x.hour << ":";
    else os << x.hour << ":";

    if ( x.minute <= 9 ) os << "0" << x.minute << ":";
    else os << x.minute << ":";

    if ( x.sec <= 9 ) os << "0" << x.sec;
    else os << x.sec;

    return os;
}

//=================================================================================================
class CMailBody {
public:
    CMailBody(int size,
              const char *data);

    // copy cons/op=/destructor is correctly implemented in the testing environment
    friend ostream &operator<<(ostream &os,
                               const CMailBody &x) {
        return os << "mail body: " << x . m_Size << " B";
    }

private:
    int m_Size;
    char *m_Data;
};

CMailBody::CMailBody(int size, const char *data) {
    this->m_Size = size;
    this->m_Data = strdup(data);
}

//=================================================================================================
class CAttach
{
public:
    CAttach (int x ) : m_X (x), m_RefCnt ( 1 )
    { }
    void           AddRef                                  ( void ) const
    {
        m_RefCnt ++;
    }
    void           Release                                 ( void ) const
    {
        if ( !--m_RefCnt )
            delete this;
    }
private:
    int            m_X;
    mutable int    m_RefCnt;
    CAttach                                 ( const CAttach   & x );
    CAttach      & operator =                              ( const CAttach   & x );
    ~CAttach                                 ( void ) = default;
    friend ostream & operator <<                           ( ostream         & os,
                                                             const CAttach   & x )
    {
        return os << "attachment: " << x . m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */


class CMail {
public:
    CMail(const CTimeStamp &timeStamp,
          const string &from,
          const CMailBody &body,
          const CAttach *attach);
    //jeste copy constructor a operator =

    //gettery
    const string &From(void) const;

    const CMailBody &Body(void) const;

    const CTimeStamp &TimeStamp(void) const;

    const CAttach *Attachment(void) const;

    friend ostream &operator<<(ostream &os,
                               const CMail &x);

    friend bool operator < (const CMail &mail, const CMail &otherMail);
    friend bool operator < (const CMail &mail, const CTimeStamp &timeStamp);
    friend bool operator < (const CTimeStamp &timeStamp, const CMail &mail);

private:
    CTimeStamp m_timeStamp;
    string m_from;
    CMailBody m_body;
    CAttach *m_attach;
};

CMail::CMail(const CTimeStamp &timeStamp, const string &from, const CMailBody &body, const CAttach *attach)
        : m_timeStamp(timeStamp), m_from(from), m_body(body) {
    this->m_attach = (CAttach *) attach;
    if ( attach != nullptr ) {
        this -> m_attach->AddRef();
    }
}

const string &CMail::From(void) const {
    return this->m_from;
}

const CMailBody &CMail::Body(void) const {
    return this->m_body;
}

const CTimeStamp &CMail::TimeStamp(void) const {
    return this->m_timeStamp;
}

const CAttach *CMail::Attachment(void) const {
    return this->m_attach;
}

ostream &operator<<(ostream &os, const CMail &x) {
   // "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
   // "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
    os << x.m_timeStamp << " " << x.m_from << " " << x.m_body;
    if ( x.m_attach != nullptr ) {
        os << " + " << *x.Attachment();
    }
    return os;
}

bool operator<(const CMail &mail, const CMail &otherMail) {
    return mail.TimeStamp().Compare(otherMail.TimeStamp()) < 0;
}

bool operator<(const CMail &mail, const CTimeStamp &timeStamp) {
    return mail.TimeStamp().Compare(timeStamp) < 0;
}

bool operator<(const CTimeStamp &timeStamp, const CMail &mail) {
    return timeStamp.Compare(mail.TimeStamp()) < 0;
}

//=================================================================================================
class CMailBox //delam emailovou schranku jednoho uzivatele
{
public:
    CMailBox(void);

    bool NewMail ( const CMail & mail , const string & toFolder );

    bool Delivery(const CMail &mail);

    bool NewFolder(const string &folderName);

    bool MoveMail(const string &fromFolder,
                  const string &toFolder);

    list<CMail> ListMail(const string &folderName,
                         const CTimeStamp &from,
                         const CTimeStamp &to) const;

    set<string> ListAddr(const CTimeStamp &from,
                         const CTimeStamp &to) const;

private:
    map<std::string, vector<CMail>> mailBox;
};

CMailBox::CMailBox(void) {
    mailBox.clear();
    vector<CMail> newVector;
    mailBox.insert(pair<string, vector<CMail>>("inbox", newVector)); // pocatecni slozka PRICHOZI POSTA
}

bool CMailBox::NewMail ( const CMail & mail, const string &toFolder ){
    if ( mailBox[toFolder].empty() ) {
        mailBox[toFolder].push_back(mail);
    }else{
        vector<CMail>::iterator iter;
        iter = lower_bound (mailBox[toFolder].begin(), mailBox[toFolder].end(), mail);
        mailBox[toFolder].insert(iter, mail);
    }

    return true;
}

bool CMailBox::Delivery(const CMail &mail) {
    return NewMail(mail, "inbox");
}

bool CMailBox::NewFolder(const string &folderName) {
    map<string, vector<CMail>>::iterator it;
    it = mailBox.find(folderName);
    if (it != mailBox.end()) return false;

    vector<CMail> newVector;
    mailBox.insert(pair<string, vector<CMail>>(folderName, newVector));
    return true;
}

bool CMailBox::MoveMail(const string &fromFolder, const string &toFolder) {
    map<string, vector<CMail>>::iterator it;
    it = mailBox.find(fromFolder);
    if (it == mailBox.end()) return false;
    it = mailBox.find(toFolder);
    if (it == mailBox.end()) return false;

    for ( const CMail& mail : mailBox[fromFolder] )
        NewMail( mail, toFolder );

    mailBox[fromFolder].clear();

    return true;
}

list<CMail> CMailBox::ListMail(const string &folderName, const CTimeStamp &from, const CTimeStamp &to) const {
    list<CMail> mailList;
    map<string, vector<CMail>>::const_iterator it;
    it = mailBox.find(folderName);

    vector<CMail>::const_iterator lowerBound;
    vector<CMail>::const_iterator upperBound;

    lowerBound = lower_bound(it->second.begin(), it->second.end(), from);
    upperBound = upper_bound(it->second.begin(), it->second.end(), to);

    for ( auto correctMails = lowerBound; correctMails != upperBound; correctMails++ ){
        mailList.push_back(*correctMails);
    }
    return mailList;
}

set<string> CMailBox::ListAddr(const CTimeStamp &from, const CTimeStamp &to) const {
    set<string> addresses;
    vector<CMail>::const_iterator lowerBound;
    vector<CMail>::const_iterator upperBound;

    for ( auto jit = mailBox.begin(); jit != mailBox.end(); jit++ ){ //tady musim projit vsechny schranky, takze smula

        lowerBound = lower_bound(jit->second.begin(), jit->second.end(), from);
        upperBound = upper_bound(jit->second.begin(), jit->second.end(), to);

        for ( auto correctMails = lowerBound; correctMails != upperBound; correctMails++ ){ //tady uz mam jenom korektni maily
            set<string>::iterator blit;
            blit = addresses.lower_bound(correctMails.operator*().From()); //tady hledam kam je zaradit
            addresses.insert(blit, correctMails.operator*().From());
        }
    }

    return addresses;
}
//=================================================================================================
#ifndef __PROGTEST__

static string showMail(const list<CMail> &l) {
    ostringstream oss;
    for (const auto &x : l){
        oss << x << endl;
    }
    return oss.str();
}

static string showUsers(const set<string> &s) {
    ostringstream oss;
    for (const auto &x : s)
        oss << x << endl;
    return oss.str();
}

int main(void) {
    list<CMail> mailList;
    set<string> users;
    CAttach *att;

    CMailBox m0;
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), nullptr)));
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"),
                  nullptr)));
    att = new CAttach(200);
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), att)));
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 18, 52, 27), "user1@fit.cvut.cz", CMailBody(14, "mail content 2"), att)));
    att->Release();
    att = new CAttach(97);
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 16, 12, 48), "boss1@fit.cvut.cz", CMailBody(24, "even more urgent message"),
                  att)));
    att->Release();
    assert (showMail(m0.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n");
    assert (showMail(m0.ListMail("inbox",
                                 CTimeStamp(2014, 3, 31, 15, 26, 23),
                                 CTimeStamp(2014, 3, 31, 16, 12, 48))) ==
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n");
    assert (showUsers(m0.ListAddr(CTimeStamp(2000, 1, 1, 0, 0, 0),
                                  CTimeStamp(2050, 12, 31, 23, 59, 59))) == "boss1@fit.cvut.cz\n"
                                                                            "user1@fit.cvut.cz\n"
                                                                            "user2@fit.cvut.cz\n");
    assert (showUsers(m0.ListAddr(CTimeStamp(2014, 3, 31, 15, 26, 23),
                                  CTimeStamp(2014, 3, 31, 16, 12, 48))) == "boss1@fit.cvut.cz\n"
                                                                           "user2@fit.cvut.cz\n");

    CMailBox m1;
    assert (m1.NewFolder("work"));
    assert (m1.NewFolder("spam"));
    assert (!m1.NewFolder("spam"));
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), nullptr)));
    att = new CAttach(500);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"),
                  att)));
    att->Release();
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), nullptr)));
    att = new CAttach(468);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 18, 52, 27), "user1@fit.cvut.cz", CMailBody(14, "mail content 2"), att)));
    att->Release();
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 16, 12, 48), "boss1@fit.cvut.cz", CMailBody(24, "even more urgent message"),
                  nullptr)));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (m1.MoveMail("inbox", "work"));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 19, 24, 13), "user2@fit.cvut.cz", CMailBody(14, "mail content 4"), nullptr)));
    att = new CAttach(234);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 13, 26, 23), "user3@fit.cvut.cz", CMailBody(9, "complains"), att)));
    att->Release();
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
            "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (m1.MoveMail("inbox", "work"));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
            "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n");
    return 0;
}

#endif /* __PROGTEST__ */