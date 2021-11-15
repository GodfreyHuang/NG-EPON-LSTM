//
// Generated file, do not edit! Created by nedtool 5.6 from traffic_gen/Messages/EPON_messages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "EPON_messages_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(MyPacket)

MyPacket::MyPacket(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->SrcAddr = 0;
    this->DestAddr = 0;
    this->Priority = 0;
    this->txEnd = 0;
    this->txStart = 0;
    this->lastPkt = false;
    this->cycle = 0;
    this->cycleZ = 0;
    this->onuIndex = 0;
}

MyPacket::MyPacket(const MyPacket& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

MyPacket::~MyPacket()
{
}

MyPacket& MyPacket::operator=(const MyPacket& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MyPacket::copy(const MyPacket& other)
{
    this->SrcAddr = other.SrcAddr;
    this->DestAddr = other.DestAddr;
    this->Priority = other.Priority;
    this->txEnd = other.txEnd;
    this->txStart = other.txStart;
    this->lastPkt = other.lastPkt;
    this->cycle = other.cycle;
    this->cycleZ = other.cycleZ;
    this->onuIndex = other.onuIndex;
}

void MyPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->SrcAddr);
    doParsimPacking(b,this->DestAddr);
    doParsimPacking(b,this->Priority);
    doParsimPacking(b,this->txEnd);
    doParsimPacking(b,this->txStart);
    doParsimPacking(b,this->lastPkt);
    doParsimPacking(b,this->cycle);
    doParsimPacking(b,this->cycleZ);
    doParsimPacking(b,this->onuIndex);
}

void MyPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->SrcAddr);
    doParsimUnpacking(b,this->DestAddr);
    doParsimUnpacking(b,this->Priority);
    doParsimUnpacking(b,this->txEnd);
    doParsimUnpacking(b,this->txStart);
    doParsimUnpacking(b,this->lastPkt);
    doParsimUnpacking(b,this->cycle);
    doParsimUnpacking(b,this->cycleZ);
    doParsimUnpacking(b,this->onuIndex);
}

uint16_t MyPacket::getSrcAddr() const
{
    return this->SrcAddr;
}

void MyPacket::setSrcAddr(uint16_t SrcAddr)
{
    this->SrcAddr = SrcAddr;
}

uint16_t MyPacket::getDestAddr() const
{
    return this->DestAddr;
}

void MyPacket::setDestAddr(uint16_t DestAddr)
{
    this->DestAddr = DestAddr;
}

uint16_t MyPacket::getPriority() const
{
    return this->Priority;
}

void MyPacket::setPriority(uint16_t Priority)
{
    this->Priority = Priority;
}

::omnetpp::simtime_t MyPacket::getTxEnd() const
{
    return this->txEnd;
}

void MyPacket::setTxEnd(::omnetpp::simtime_t txEnd)
{
    this->txEnd = txEnd;
}

::omnetpp::simtime_t MyPacket::getTxStart() const
{
    return this->txStart;
}

void MyPacket::setTxStart(::omnetpp::simtime_t txStart)
{
    this->txStart = txStart;
}

bool MyPacket::getLastPkt() const
{
    return this->lastPkt;
}

void MyPacket::setLastPkt(bool lastPkt)
{
    this->lastPkt = lastPkt;
}

uint32_t MyPacket::getCycle() const
{
    return this->cycle;
}

void MyPacket::setCycle(uint32_t cycle)
{
    this->cycle = cycle;
}

uint32_t MyPacket::getCycleZ() const
{
    return this->cycleZ;
}

void MyPacket::setCycleZ(uint32_t cycleZ)
{
    this->cycleZ = cycleZ;
}

uint16_t MyPacket::getOnuIndex() const
{
    return this->onuIndex;
}

void MyPacket::setOnuIndex(uint16_t onuIndex)
{
    this->onuIndex = onuIndex;
}

class MyPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MyPacketDescriptor();
    virtual ~MyPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MyPacketDescriptor)

MyPacketDescriptor::MyPacketDescriptor() : omnetpp::cClassDescriptor("MyPacket", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

MyPacketDescriptor::~MyPacketDescriptor()
{
    delete[] propertynames;
}

bool MyPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MyPacket *>(obj)!=nullptr;
}

const char **MyPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MyPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MyPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int MyPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *MyPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "SrcAddr",
        "DestAddr",
        "Priority",
        "txEnd",
        "txStart",
        "lastPkt",
        "cycle",
        "cycleZ",
        "onuIndex",
    };
    return (field>=0 && field<9) ? fieldNames[field] : nullptr;
}

int MyPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='S' && strcmp(fieldName, "SrcAddr")==0) return base+0;
    if (fieldName[0]=='D' && strcmp(fieldName, "DestAddr")==0) return base+1;
    if (fieldName[0]=='P' && strcmp(fieldName, "Priority")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "txEnd")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "txStart")==0) return base+4;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastPkt")==0) return base+5;
    if (fieldName[0]=='c' && strcmp(fieldName, "cycle")==0) return base+6;
    if (fieldName[0]=='c' && strcmp(fieldName, "cycleZ")==0) return base+7;
    if (fieldName[0]=='o' && strcmp(fieldName, "onuIndex")==0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MyPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint16_t",
        "uint16_t",
        "simtime_t",
        "simtime_t",
        "bool",
        "uint32_t",
        "uint32_t",
        "uint16_t",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : nullptr;
}

const char **MyPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MyPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MyPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MyPacketDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MyPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSrcAddr());
        case 1: return ulong2string(pp->getDestAddr());
        case 2: return ulong2string(pp->getPriority());
        case 3: return simtime2string(pp->getTxEnd());
        case 4: return simtime2string(pp->getTxStart());
        case 5: return bool2string(pp->getLastPkt());
        case 6: return ulong2string(pp->getCycle());
        case 7: return ulong2string(pp->getCycleZ());
        case 8: return ulong2string(pp->getOnuIndex());
        default: return "";
    }
}

bool MyPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcAddr(string2ulong(value)); return true;
        case 1: pp->setDestAddr(string2ulong(value)); return true;
        case 2: pp->setPriority(string2ulong(value)); return true;
        case 3: pp->setTxEnd(string2simtime(value)); return true;
        case 4: pp->setTxStart(string2simtime(value)); return true;
        case 5: pp->setLastPkt(string2bool(value)); return true;
        case 6: pp->setCycle(string2ulong(value)); return true;
        case 7: pp->setCycleZ(string2ulong(value)); return true;
        case 8: pp->setOnuIndex(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MyPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MyPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(MPCP)

MPCP::MPCP(const char *name, short kind) : ::MyPacket(name,kind)
{
    this->opcode = 0;
    this->ts = 0;
}

MPCP::MPCP(const MPCP& other) : ::MyPacket(other)
{
    copy(other);
}

MPCP::~MPCP()
{
}

MPCP& MPCP::operator=(const MPCP& other)
{
    if (this==&other) return *this;
    ::MyPacket::operator=(other);
    copy(other);
    return *this;
}

void MPCP::copy(const MPCP& other)
{
    this->opcode = other.opcode;
    this->ts = other.ts;
}

void MPCP::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MyPacket::parsimPack(b);
    doParsimPacking(b,this->opcode);
    doParsimPacking(b,this->ts);
}

void MPCP::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MyPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->opcode);
    doParsimUnpacking(b,this->ts);
}

uint16_t MPCP::getOpcode() const
{
    return this->opcode;
}

void MPCP::setOpcode(uint16_t opcode)
{
    this->opcode = opcode;
}

uint32_t MPCP::getTs() const
{
    return this->ts;
}

void MPCP::setTs(uint32_t ts)
{
    this->ts = ts;
}

class MPCPDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MPCPDescriptor();
    virtual ~MPCPDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MPCPDescriptor)

MPCPDescriptor::MPCPDescriptor() : omnetpp::cClassDescriptor("MPCP", "MyPacket")
{
    propertynames = nullptr;
}

MPCPDescriptor::~MPCPDescriptor()
{
    delete[] propertynames;
}

bool MPCPDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MPCP *>(obj)!=nullptr;
}

const char **MPCPDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MPCPDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MPCPDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int MPCPDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "opcode",
        "ts",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int MPCPDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "opcode")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "ts")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MPCPDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint32_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **MPCPDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MPCPDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MPCPDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MPCPDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MPCPDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getOpcode());
        case 1: return ulong2string(pp->getTs());
        default: return "";
    }
}

bool MPCPDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: pp->setOpcode(string2ulong(value)); return true;
        case 1: pp->setTs(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MPCPDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RTTReg)

RTTReg::RTTReg(const char *name, short kind) : ::MPCP(name,kind)
{
    this->Rtt = 0;
}

RTTReg::RTTReg(const RTTReg& other) : ::MPCP(other)
{
    copy(other);
}

RTTReg::~RTTReg()
{
}

RTTReg& RTTReg::operator=(const RTTReg& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void RTTReg::copy(const RTTReg& other)
{
    this->Rtt = other.Rtt;
}

void RTTReg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MPCP::parsimPack(b);
    doParsimPacking(b,this->Rtt);
}

void RTTReg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doParsimUnpacking(b,this->Rtt);
}

::omnetpp::simtime_t RTTReg::getRtt() const
{
    return this->Rtt;
}

void RTTReg::setRtt(::omnetpp::simtime_t Rtt)
{
    this->Rtt = Rtt;
}

class RTTRegDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RTTRegDescriptor();
    virtual ~RTTRegDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(RTTRegDescriptor)

RTTRegDescriptor::RTTRegDescriptor() : omnetpp::cClassDescriptor("RTTReg", "MPCP")
{
    propertynames = nullptr;
}

RTTRegDescriptor::~RTTRegDescriptor()
{
    delete[] propertynames;
}

bool RTTRegDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RTTReg *>(obj)!=nullptr;
}

const char **RTTRegDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RTTRegDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RTTRegDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RTTRegDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *RTTRegDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "Rtt",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RTTRegDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='R' && strcmp(fieldName, "Rtt")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RTTRegDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **RTTRegDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RTTRegDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RTTRegDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RTTRegDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RTTRegDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        case 0: return simtime2string(pp->getRtt());
        default: return "";
    }
}

bool RTTRegDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        case 0: pp->setRtt(string2simtime(value)); return true;
        default: return false;
    }
}

const char *RTTRegDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *RTTRegDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RTTReg *pp = (RTTReg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(MPCPAutoDiscovery)

MPCPAutoDiscovery::MPCPAutoDiscovery(const char *name, short kind) : ::MPCP(name,kind)
{
    this->rtt = 0;
}

MPCPAutoDiscovery::MPCPAutoDiscovery(const MPCPAutoDiscovery& other) : ::MPCP(other)
{
    copy(other);
}

MPCPAutoDiscovery::~MPCPAutoDiscovery()
{
}

MPCPAutoDiscovery& MPCPAutoDiscovery::operator=(const MPCPAutoDiscovery& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPAutoDiscovery::copy(const MPCPAutoDiscovery& other)
{
    this->rtt = other.rtt;
}

void MPCPAutoDiscovery::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MPCP::parsimPack(b);
    doParsimPacking(b,this->rtt);
}

void MPCPAutoDiscovery::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doParsimUnpacking(b,this->rtt);
}

::omnetpp::simtime_t MPCPAutoDiscovery::getRtt() const
{
    return this->rtt;
}

void MPCPAutoDiscovery::setRtt(::omnetpp::simtime_t rtt)
{
    this->rtt = rtt;
}

class MPCPAutoDiscoveryDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MPCPAutoDiscoveryDescriptor();
    virtual ~MPCPAutoDiscoveryDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MPCPAutoDiscoveryDescriptor)

MPCPAutoDiscoveryDescriptor::MPCPAutoDiscoveryDescriptor() : omnetpp::cClassDescriptor("MPCPAutoDiscovery", "MPCP")
{
    propertynames = nullptr;
}

MPCPAutoDiscoveryDescriptor::~MPCPAutoDiscoveryDescriptor()
{
    delete[] propertynames;
}

bool MPCPAutoDiscoveryDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MPCPAutoDiscovery *>(obj)!=nullptr;
}

const char **MPCPAutoDiscoveryDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MPCPAutoDiscoveryDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MPCPAutoDiscoveryDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int MPCPAutoDiscoveryDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *MPCPAutoDiscoveryDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "rtt",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int MPCPAutoDiscoveryDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "rtt")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MPCPAutoDiscoveryDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **MPCPAutoDiscoveryDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MPCPAutoDiscoveryDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MPCPAutoDiscoveryDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MPCPAutoDiscoveryDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MPCPAutoDiscoveryDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        case 0: return simtime2string(pp->getRtt());
        default: return "";
    }
}

bool MPCPAutoDiscoveryDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        case 0: pp->setRtt(string2simtime(value)); return true;
        default: return false;
    }
}

const char *MPCPAutoDiscoveryDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MPCPAutoDiscoveryDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MPCPAutoDiscovery *pp = (MPCPAutoDiscovery *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(MPCPGate)

MPCPGate::MPCPGate(const char *name, short kind) : ::MPCP(name,kind)
{
    this->startTime = 0;
    this->upLength = 0;
    this->downLength = 0;
    for (unsigned int i=0; i<2; i++)
        this->queueLength[i] = 0;
    for (unsigned int i=0; i<2; i++)
        this->pwsTime[i] = 0;
    for (unsigned int i=0; i<2; i++)
        this->pwsMode[i] = 0;
    this->interSleep = false;
    this->endOfCycleTime = 0;
    this->endOfInterSleep = 0;
    this->transmitChannel = 0;
    this->onuIndex = 0;
    this->holded = false;
    this->recordedEnergyMode = 0;
}

MPCPGate::MPCPGate(const MPCPGate& other) : ::MPCP(other)
{
    copy(other);
}

MPCPGate::~MPCPGate()
{
}

MPCPGate& MPCPGate::operator=(const MPCPGate& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPGate::copy(const MPCPGate& other)
{
    this->startTime = other.startTime;
    this->upLength = other.upLength;
    this->downLength = other.downLength;
    for (unsigned int i=0; i<2; i++)
        this->queueLength[i] = other.queueLength[i];
    for (unsigned int i=0; i<2; i++)
        this->pwsTime[i] = other.pwsTime[i];
    for (unsigned int i=0; i<2; i++)
        this->pwsMode[i] = other.pwsMode[i];
    this->interSleep = other.interSleep;
    this->endOfCycleTime = other.endOfCycleTime;
    this->endOfInterSleep = other.endOfInterSleep;
    this->transmitChannel = other.transmitChannel;
    this->onuIndex = other.onuIndex;
    this->holded = other.holded;
    this->recordedEnergyMode = other.recordedEnergyMode;
}

void MPCPGate::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MPCP::parsimPack(b);
    doParsimPacking(b,this->startTime);
    doParsimPacking(b,this->upLength);
    doParsimPacking(b,this->downLength);
    doParsimArrayPacking(b,this->queueLength,2);
    doParsimArrayPacking(b,this->pwsTime,2);
    doParsimArrayPacking(b,this->pwsMode,2);
    doParsimPacking(b,this->interSleep);
    doParsimPacking(b,this->endOfCycleTime);
    doParsimPacking(b,this->endOfInterSleep);
    doParsimPacking(b,this->transmitChannel);
    doParsimPacking(b,this->onuIndex);
    doParsimPacking(b,this->holded);
    doParsimPacking(b,this->recordedEnergyMode);
}

void MPCPGate::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->upLength);
    doParsimUnpacking(b,this->downLength);
    doParsimArrayUnpacking(b,this->queueLength,2);
    doParsimArrayUnpacking(b,this->pwsTime,2);
    doParsimArrayUnpacking(b,this->pwsMode,2);
    doParsimUnpacking(b,this->interSleep);
    doParsimUnpacking(b,this->endOfCycleTime);
    doParsimUnpacking(b,this->endOfInterSleep);
    doParsimUnpacking(b,this->transmitChannel);
    doParsimUnpacking(b,this->onuIndex);
    doParsimUnpacking(b,this->holded);
    doParsimUnpacking(b,this->recordedEnergyMode);
}

::omnetpp::simtime_t MPCPGate::getStartTime() const
{
    return this->startTime;
}

void MPCPGate::setStartTime(::omnetpp::simtime_t startTime)
{
    this->startTime = startTime;
}

uint32_t MPCPGate::getUpLength() const
{
    return this->upLength;
}

void MPCPGate::setUpLength(uint32_t upLength)
{
    this->upLength = upLength;
}

uint32_t MPCPGate::getDownLength() const
{
    return this->downLength;
}

void MPCPGate::setDownLength(uint32_t downLength)
{
    this->downLength = downLength;
}

unsigned int MPCPGate::getQueueLengthArraySize() const
{
    return 2;
}

uint32_t MPCPGate::getQueueLength(unsigned int k) const
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return this->queueLength[k];
}

void MPCPGate::setQueueLength(unsigned int k, uint32_t queueLength)
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->queueLength[k] = queueLength;
}

unsigned int MPCPGate::getPwsTimeArraySize() const
{
    return 2;
}

::omnetpp::simtime_t MPCPGate::getPwsTime(unsigned int k) const
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return this->pwsTime[k];
}

void MPCPGate::setPwsTime(unsigned int k, ::omnetpp::simtime_t pwsTime)
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->pwsTime[k] = pwsTime;
}

unsigned int MPCPGate::getPwsModeArraySize() const
{
    return 2;
}

uint16_t MPCPGate::getPwsMode(unsigned int k) const
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    return this->pwsMode[k];
}

void MPCPGate::setPwsMode(unsigned int k, uint16_t pwsMode)
{
    if (k>=2) throw omnetpp::cRuntimeError("Array of size 2 indexed by %lu", (unsigned long)k);
    this->pwsMode[k] = pwsMode;
}

bool MPCPGate::getInterSleep() const
{
    return this->interSleep;
}

void MPCPGate::setInterSleep(bool interSleep)
{
    this->interSleep = interSleep;
}

::omnetpp::simtime_t MPCPGate::getEndOfCycleTime() const
{
    return this->endOfCycleTime;
}

void MPCPGate::setEndOfCycleTime(::omnetpp::simtime_t endOfCycleTime)
{
    this->endOfCycleTime = endOfCycleTime;
}

::omnetpp::simtime_t MPCPGate::getEndOfInterSleep() const
{
    return this->endOfInterSleep;
}

void MPCPGate::setEndOfInterSleep(::omnetpp::simtime_t endOfInterSleep)
{
    this->endOfInterSleep = endOfInterSleep;
}

uint16_t MPCPGate::getTransmitChannel() const
{
    return this->transmitChannel;
}

void MPCPGate::setTransmitChannel(uint16_t transmitChannel)
{
    this->transmitChannel = transmitChannel;
}

uint16_t MPCPGate::getOnuIndex() const
{
    return this->onuIndex;
}

void MPCPGate::setOnuIndex(uint16_t onuIndex)
{
    this->onuIndex = onuIndex;
}

bool MPCPGate::getHolded() const
{
    return this->holded;
}

void MPCPGate::setHolded(bool holded)
{
    this->holded = holded;
}

uint16_t MPCPGate::getRecordedEnergyMode() const
{
    return this->recordedEnergyMode;
}

void MPCPGate::setRecordedEnergyMode(uint16_t recordedEnergyMode)
{
    this->recordedEnergyMode = recordedEnergyMode;
}

class MPCPGateDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MPCPGateDescriptor();
    virtual ~MPCPGateDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MPCPGateDescriptor)

MPCPGateDescriptor::MPCPGateDescriptor() : omnetpp::cClassDescriptor("MPCPGate", "MPCP")
{
    propertynames = nullptr;
}

MPCPGateDescriptor::~MPCPGateDescriptor()
{
    delete[] propertynames;
}

bool MPCPGateDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MPCPGate *>(obj)!=nullptr;
}

const char **MPCPGateDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MPCPGateDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MPCPGateDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 13+basedesc->getFieldCount() : 13;
}

unsigned int MPCPGateDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<13) ? fieldTypeFlags[field] : 0;
}

const char *MPCPGateDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "startTime",
        "upLength",
        "downLength",
        "queueLength",
        "pwsTime",
        "pwsMode",
        "interSleep",
        "endOfCycleTime",
        "endOfInterSleep",
        "transmitChannel",
        "onuIndex",
        "holded",
        "recordedEnergyMode",
    };
    return (field>=0 && field<13) ? fieldNames[field] : nullptr;
}

int MPCPGateDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "startTime")==0) return base+0;
    if (fieldName[0]=='u' && strcmp(fieldName, "upLength")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "downLength")==0) return base+2;
    if (fieldName[0]=='q' && strcmp(fieldName, "queueLength")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "pwsTime")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "pwsMode")==0) return base+5;
    if (fieldName[0]=='i' && strcmp(fieldName, "interSleep")==0) return base+6;
    if (fieldName[0]=='e' && strcmp(fieldName, "endOfCycleTime")==0) return base+7;
    if (fieldName[0]=='e' && strcmp(fieldName, "endOfInterSleep")==0) return base+8;
    if (fieldName[0]=='t' && strcmp(fieldName, "transmitChannel")==0) return base+9;
    if (fieldName[0]=='o' && strcmp(fieldName, "onuIndex")==0) return base+10;
    if (fieldName[0]=='h' && strcmp(fieldName, "holded")==0) return base+11;
    if (fieldName[0]=='r' && strcmp(fieldName, "recordedEnergyMode")==0) return base+12;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MPCPGateDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "simtime_t",
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "simtime_t",
        "uint16_t",
        "bool",
        "simtime_t",
        "simtime_t",
        "uint16_t",
        "uint16_t",
        "bool",
        "uint16_t",
    };
    return (field>=0 && field<13) ? fieldTypeStrings[field] : nullptr;
}

const char **MPCPGateDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MPCPGateDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MPCPGateDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 3: return 2;
        case 4: return 2;
        case 5: return 2;
        default: return 0;
    }
}

const char *MPCPGateDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MPCPGateDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: return simtime2string(pp->getStartTime());
        case 1: return ulong2string(pp->getUpLength());
        case 2: return ulong2string(pp->getDownLength());
        case 3: return ulong2string(pp->getQueueLength(i));
        case 4: return simtime2string(pp->getPwsTime(i));
        case 5: return ulong2string(pp->getPwsMode(i));
        case 6: return bool2string(pp->getInterSleep());
        case 7: return simtime2string(pp->getEndOfCycleTime());
        case 8: return simtime2string(pp->getEndOfInterSleep());
        case 9: return ulong2string(pp->getTransmitChannel());
        case 10: return ulong2string(pp->getOnuIndex());
        case 11: return bool2string(pp->getHolded());
        case 12: return ulong2string(pp->getRecordedEnergyMode());
        default: return "";
    }
}

bool MPCPGateDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: pp->setStartTime(string2simtime(value)); return true;
        case 1: pp->setUpLength(string2ulong(value)); return true;
        case 2: pp->setDownLength(string2ulong(value)); return true;
        case 3: pp->setQueueLength(i,string2ulong(value)); return true;
        case 4: pp->setPwsTime(i,string2simtime(value)); return true;
        case 5: pp->setPwsMode(i,string2ulong(value)); return true;
        case 6: pp->setInterSleep(string2bool(value)); return true;
        case 7: pp->setEndOfCycleTime(string2simtime(value)); return true;
        case 8: pp->setEndOfInterSleep(string2simtime(value)); return true;
        case 9: pp->setTransmitChannel(string2ulong(value)); return true;
        case 10: pp->setOnuIndex(string2ulong(value)); return true;
        case 11: pp->setHolded(string2bool(value)); return true;
        case 12: pp->setRecordedEnergyMode(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPGateDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MPCPGateDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(MPCPReport)

MPCPReport::MPCPReport(const char *name, short kind) : ::MPCP(name,kind)
{
    this->curMode = 0;
    this->requestSize = 0;
}

MPCPReport::MPCPReport(const MPCPReport& other) : ::MPCP(other)
{
    copy(other);
}

MPCPReport::~MPCPReport()
{
}

MPCPReport& MPCPReport::operator=(const MPCPReport& other)
{
    if (this==&other) return *this;
    ::MPCP::operator=(other);
    copy(other);
    return *this;
}

void MPCPReport::copy(const MPCPReport& other)
{
    this->curMode = other.curMode;
    this->requestSize = other.requestSize;
}

void MPCPReport::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MPCP::parsimPack(b);
    doParsimPacking(b,this->curMode);
    doParsimPacking(b,this->requestSize);
}

void MPCPReport::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MPCP::parsimUnpack(b);
    doParsimUnpacking(b,this->curMode);
    doParsimUnpacking(b,this->requestSize);
}

uint16_t MPCPReport::getCurMode() const
{
    return this->curMode;
}

void MPCPReport::setCurMode(uint16_t curMode)
{
    this->curMode = curMode;
}

uint32_t MPCPReport::getRequestSize() const
{
    return this->requestSize;
}

void MPCPReport::setRequestSize(uint32_t requestSize)
{
    this->requestSize = requestSize;
}

class MPCPReportDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MPCPReportDescriptor();
    virtual ~MPCPReportDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MPCPReportDescriptor)

MPCPReportDescriptor::MPCPReportDescriptor() : omnetpp::cClassDescriptor("MPCPReport", "MPCP")
{
    propertynames = nullptr;
}

MPCPReportDescriptor::~MPCPReportDescriptor()
{
    delete[] propertynames;
}

bool MPCPReportDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MPCPReport *>(obj)!=nullptr;
}

const char **MPCPReportDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MPCPReportDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MPCPReportDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int MPCPReportDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPReportDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "curMode",
        "requestSize",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int MPCPReportDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "curMode")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestSize")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MPCPReportDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint32_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **MPCPReportDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MPCPReportDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MPCPReportDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MPCPReportDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MPCPReportDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getCurMode());
        case 1: return ulong2string(pp->getRequestSize());
        default: return "";
    }
}

bool MPCPReportDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: pp->setCurMode(string2ulong(value)); return true;
        case 1: pp->setRequestSize(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPReportDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MPCPReportDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(TimeOutMsg)

TimeOutMsg::TimeOutMsg(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->onuIdx = 0;
}

TimeOutMsg::TimeOutMsg(const TimeOutMsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

TimeOutMsg::~TimeOutMsg()
{
}

TimeOutMsg& TimeOutMsg::operator=(const TimeOutMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void TimeOutMsg::copy(const TimeOutMsg& other)
{
    this->onuIdx = other.onuIdx;
}

void TimeOutMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->onuIdx);
}

void TimeOutMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->onuIdx);
}

uint16_t TimeOutMsg::getOnuIdx() const
{
    return this->onuIdx;
}

void TimeOutMsg::setOnuIdx(uint16_t onuIdx)
{
    this->onuIdx = onuIdx;
}

class TimeOutMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TimeOutMsgDescriptor();
    virtual ~TimeOutMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TimeOutMsgDescriptor)

TimeOutMsgDescriptor::TimeOutMsgDescriptor() : omnetpp::cClassDescriptor("TimeOutMsg", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

TimeOutMsgDescriptor::~TimeOutMsgDescriptor()
{
    delete[] propertynames;
}

bool TimeOutMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TimeOutMsg *>(obj)!=nullptr;
}

const char **TimeOutMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TimeOutMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TimeOutMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int TimeOutMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *TimeOutMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "onuIdx",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int TimeOutMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "onuIdx")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TimeOutMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **TimeOutMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TimeOutMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TimeOutMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TimeOutMsg *pp = (TimeOutMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *TimeOutMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TimeOutMsg *pp = (TimeOutMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TimeOutMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TimeOutMsg *pp = (TimeOutMsg *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getOnuIdx());
        default: return "";
    }
}

bool TimeOutMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TimeOutMsg *pp = (TimeOutMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setOnuIdx(string2ulong(value)); return true;
        default: return false;
    }
}

const char *TimeOutMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *TimeOutMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TimeOutMsg *pp = (TimeOutMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


