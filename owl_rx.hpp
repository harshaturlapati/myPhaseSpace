// owl_rx.hpp -*- C++ -*-
// OWL C++ API v2.0

#ifndef OWL_RX_HPP
#define OWL_RX_HPP

#include <iostream>
#include <map>

#include <string.h>

namespace OWL {

  //// HubPacket ////

  struct HubPacket {
    uint8_t inputTTL;
    uint8_t payloadPage;
    uint8_t payload[64];
  };

  //// RXPacket ////

  struct RXPacket {

    enum { State=1, RFScan=2, RFData=3 };

    enum {
      CAMFLAGS_STATE = 1<<7,
      CAMFLAGS_RXSLOT = 1<<6,
      CAMFLAGS_RF_PIPE = 3<<0,
    };

    enum {
      PACKET_TYPE_MASK = 1<<7,
      PACKET_TYPE_SCAN = 0<<7,
      PACKET_TYPE_DATA = 1<<7
    };

    uint8_t hwid[2];
    uint8_t hwtype;
    uint8_t id;
    uint8_t rxMeta[4];
    uint8_t camFlags;
    uint8_t camStatus;
    uint8_t data[18];

    operator int() const { return (hwid[0] << 8) | (hwid[1] << 0); }

    int type() const;
  };

  //// CamStatePacket ////

  struct CamStatePacket {
    uint8_t ledSlot; // backup
    uint8_t rxSlot[3]; // 0:config, 1:state, 2:scan addr
    uint8_t txFlags[2]; // handshake for camera tx's
    uint8_t updateToggle;  // status of toggle  bits
    uint8_t rfTxAlternate; // state of rf tx masking
    uint8_t data[10];
  };

  //// RFScanPacket ////

  struct RFScanPacket {
    uint8_t type;
    uint8_t hwid[2];
    uint8_t hwtype;
    uint8_t rxAddr[2];
    uint8_t signalStatus;
    uint8_t batteryStatus;
    uint8_t encodeStatus;
    uint8_t readoutAddr;
    uint8_t readout[8];
  };

  //// RFDataPacket ////

  struct RFDataPacket {
    uint8_t type;
    uint8_t retry;
    uint8_t data[16];
  };

  //// RFDevice ////

  struct RFDevice {

    typedef void (*update_function)(OWL::RFDevice &dev, const RXPacket &rx, int64_t time);

    uint16_t hwid;
    uint8_t type;
    uint8_t id;

    int64_t lastUpdated, lastChanged;

    RFDevice();
    RFDevice(const RXPacket &rx, int64_t time);
    RFDevice(const RFDevice &d);

    void update(const RXPacket &rx, int64_t time);

    bool isValid() const { return lastChanged > -1; }
  };

  //// RFDevices_ ////

  template <typename Device>
  class RFDevices_ : public std::map<int,Device> {
  public:
    typedef std::map<int,Device> base;

    int verbose;

    RFDevice::update_function update_callback;

    RFDevices_();

    size_t update(const OWL::Event *inputs, int64_t timeout=5000);
    void update(const RXPacket &rx, int64_t time, int64_t timeout);

    void info(int64_t time=-1) const;
  };

  typedef RFDevices_<RFDevice> RFDevices;

  ////

  //// RXPacket ////

  inline int RXPacket::type() const
  {
    if(camFlags & CAMFLAGS_STATE) return State;
    if((camFlags & CAMFLAGS_RF_PIPE) != 0) return 0;
    return (data[0] & PACKET_TYPE_MASK) == PACKET_TYPE_SCAN ? RFScan : RFData;
  }

  //// RFDevice ////

  inline RFDevice::RFDevice() : hwid(0), type(-1), id(-1), lastUpdated(-1), lastChanged(-1)
  {
  }

  inline RFDevice::RFDevice(const RXPacket &rx, int64_t time) :
    hwid(rx), type(rx.hwtype), id(rx.id), lastUpdated(time), lastChanged(time)
  {
  }

  inline RFDevice::RFDevice(const RFDevice &d) :
    hwid(d.hwid), type(d.type), id(d.id), lastUpdated(d.lastUpdated), lastChanged(d.lastChanged)
  {
  }

  inline void RFDevice::update(const RXPacket &rx, int64_t time)
  {
    if(type != rx.hwtype || id != rx.id) lastChanged = time;
    type = rx.hwtype;
    id = rx.id;
    lastUpdated = time;
  }

  //// RFDevices ////

  template <typename Device>
  RFDevices_<Device>::RFDevices_() : verbose(1), update_callback(0)
  {
  }

  template <typename Device>
  size_t RFDevices_<Device>::update(const OWL::Event *inputs, int64_t timeout)
  {
    if(!inputs) return 0;

    size_t count = 0;
    for(const OWL::Input *i = inputs->begin(); i != inputs->end(); i++)
      {
        if(i->data.size() == sizeof(OWL::RXPacket))
          if(OWL::RXPacket *rx = (OWL::RXPacket*)i->data.data())
            {
              update(*rx, i->time, timeout);
            }
        count++;
      }
    return count;
  }

  template <typename Device>
  void RFDevices_<Device>::update(const RXPacket &rx, int64_t time, int64_t timeout)
  {
    if((rx.type() == RXPacket::RFScan || rx.type() == RXPacket::RFData) && rx != 0)
      {
        // find or create device to update
        typename base::iterator i = base::find(rx);
        if(i == base::end())
          {
            i = base::insert(std::make_pair<>(rx, Device(rx, time))).first;
            if(verbose) std::cout << "new device: " << std::hex << (i->second.hwid>>3) << std::dec << std::endl;
          }
        else i->second.update(rx, time);

        if(update_callback) update_callback(i->second, rx, time);
      }

    // invalidate stale devices
    for(typename base::iterator i = base::begin(); timeout > 0 && i != base::end();)
      if(i->second.isValid() && i->second.lastUpdated + timeout < time)
        {
          if(verbose) std::cout << "remove device: " << std::hex << (i->second.hwid>>3) << std::dec << std::endl;
          base::erase(i++);
        }
      else i++;
  }

  template <typename Device>
  void RFDevices_<Device>::info(int64_t time) const
  {
    if(base::empty()) return;
    if(time < 0)
      {
        size_t count = 0;
        for(typename base::const_iterator i = base::begin(); i != base::end(); i++)
          if(i->second.isValid()) count++;
        std::cout << count << " device(s):" << std::endl;
      }
    for(typename base::const_iterator i = base::begin(); i != base::end(); i++)
      {
        const RFDevice &d = i->second;
        if(d.isValid())
          {
            if(time > -1 && time != d.lastChanged) continue;
            std::cout << "device hwid=0x" << std::hex << (d.hwid>>3) << std::dec
                      << " type=" << (int)d.type
                      << " id=" << (int)(char)d.id
                      << std::endl;
          }
      }
  }

  ////

} // namespace OWL

#endif // OWL_RX_HPP
