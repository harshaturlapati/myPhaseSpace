// owl.hpp -*- C++ -*-
// OWL C++ API v2.2

/***
Copyright (c) PhaseSpace, Inc 2017

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL PHASESPACE, INC
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***/

#ifndef OWL_HPP
#define OWL_HPP

#include <cstdlib>
#include <string>
#include <vector>

#include <stdint.h>

#ifdef WIN32
#ifdef __DLL
#define OWLAPI __declspec(dllexport)
#else // !__DLL
#define OWLAPI __declspec(dllimport)
#endif // __DLL
#ifdef ERROR
#undef ERROR
#endif
#else // ! WIN32
#define OWLAPI
#endif // WIN32

#define OWL_MAX_FREQUENCY 960.0

// id is unsigned 32-bit
// time is signed 64-bit, 1 count per frame or to be specified
// pose: pos, rot -- [x y z], [s x y z]
// options format: [opt=value opt2=value1,value2 ...]

namespace OWL {

  typedef std::string string_ref;
  using std::string;
  using std::vector;

  extern const char VERSION[];

  //// Data Types ////

  struct Camera {
    uint32_t id;
    uint32_t flags;
    float pose[7];
    float cond;
  };

  struct Peak {
    uint32_t id;
    uint32_t flags;
    int64_t time;
    uint16_t camera;
    uint16_t detector;
    uint32_t width;
    float pos;
    float amp;
  };

  struct Plane {
    uint32_t id;
    uint32_t flags;
    int64_t time;
    uint16_t camera;
    uint16_t detector;
    float plane[4];
    float offset;
  };

  struct Marker {
    uint32_t id;
    uint32_t flags;
    int64_t time;
    float x, y, z;
    float cond;
  };

  struct Rigid {
    uint32_t id;
    uint32_t flags;
    int64_t time;
    float pose[7];
    float cond;
  };

  struct Input {
    uint64_t hw_id;
    uint64_t flags;
    int64_t time;
    vector<uint8_t> data;
  };

  typedef std::vector<Camera> Cameras;
  typedef std::vector<Peak> Peaks;
  typedef std::vector<Plane> Planes;
  typedef std::vector<Marker> Markers;
  typedef std::vector<Rigid> Rigids;
  typedef std::vector<Input> Inputs;

  struct Type;
  struct Variant;
  struct Event;

  //// Info Types ////

  struct OWLAPI MarkerInfo {
    uint32_t id;
    uint32_t tracker_id;
    string name;
    string options;
    MarkerInfo(uint32_t id=-1, uint32_t tracker_id=-1,
	       const string_ref &name=string_ref(),
	       const string_ref &options=string_ref());
  };

  struct OWLAPI TrackerInfo {
    uint32_t id;
    string type;
    string name;
    string options;
    vector<uint32_t> marker_ids;
    TrackerInfo(uint32_t id=-1,
                const string_ref &type=string_ref(),
		const string_ref &name=string_ref(),
		const string_ref &options=string_ref(),
		const vector<uint32_t> &marker_ids=vector<uint32_t>());
    TrackerInfo(uint32_t id,
                const string_ref &type, const string_ref &name,
                const string_ref &options, const string_ref &marker_ids);
  };

  struct OWLAPI FilterInfo {
    uint32_t delay;
    string name;
    string options;
    FilterInfo(uint32_t delay=0,
               const string_ref &name=string_ref(),
               const string_ref &options=string_ref());
  };

  struct OWLAPI DeviceInfo {
    uint64_t hw_id;
    int64_t time;
    string type;
    string name;
    string options;
    string status;
    DeviceInfo(uint64_t hw_id=0);
  };

  struct OWLAPI PackInfo {
    uint16_t type_id;
    uint16_t id;
    string name;
    string options;
    vector<uint64_t> ids;
    PackInfo(uint16_t type_id=0, const string_ref &name=string_ref(),
             const string_ref &options=string_ref(),
             const vector<uint64_t> &ids=vector<uint64_t>());
    PackInfo(uint16_t type_id, const string_ref &name,
             const string_ref &options, const string_ref &ids);
  };

  typedef std::vector<MarkerInfo> MarkerInfoTable;
  typedef std::vector<TrackerInfo> TrackerInfoTable;
  typedef std::vector<FilterInfo> FilterInfoTable;
  typedef std::vector<DeviceInfo> DeviceInfoTable;
  typedef std::vector<PackInfo> PackInfoTable;

  //// Context ////

  class ContextData;

  class OWLAPI Context {
  public:

    Context();
    ~Context();

    // initialization //

    int open(const string_ref &name, const string_ref &open_options=string_ref());
    bool close();
    bool isOpen() const;

    int initialize(const string_ref &init_options=string_ref());
    int done(const string_ref &done_options=string_ref());

    int streaming() const;
    bool streaming(int enable);

    float frequency() const;
    bool frequency(float freq);

    const int* timeBase() const;
    bool timeBase(int num, int den);

    float scale() const;
    bool scale(float scale);

    const float* pose() const;
    bool pose(const float *pose);

    string option(const string_ref &option) const;
    string options() const;
    bool option(const string_ref &option, const string_ref &value);
    bool options(const string_ref &options);

    string lastError() const;

    // markers //

    bool markerName(uint32_t marker_id, const string_ref &marker_name);
    bool markerOptions(uint32_t marker_id, const string_ref &marker_options);

    MarkerInfo markerInfo(uint32_t marker_id) const;

    // trackers //

    bool createTracker(uint32_t tracker_id, const string_ref &tracker_type,
                       const string_ref &tracker_name=string_ref(),
                       const string_ref &tracker_options=string_ref());
    bool createTrackers(const TrackerInfo *first, const TrackerInfo *last);

    bool destroyTracker(uint32_t tracker_id);
    bool destroyTrackers(const uint32_t *first, const uint32_t *last);

    bool assignMarker(uint32_t tracker_id, uint32_t marker_id,
                      const string_ref &marker_name=string_ref(),
                      const string_ref &marker_options=string_ref());
    bool assignMarkers(const MarkerInfo *first, const MarkerInfo *last);

    bool trackerName(uint32_t tracker_id, const string_ref &tracker_name);
    bool trackerOptions(uint32_t tracker_id, const string_ref &tracker_options);

    vector<TrackerInfo> findTrackerInfo(const string_ref &type, const string_ref &name=string_ref()) const;

    TrackerInfo trackerInfo(uint32_t tracker_id) const;

    // filters //

    bool filter(uint32_t delay, const string_ref &name, const string_ref &filter_options);
    bool filters(const FilterInfo *first, const FilterInfo *last);

    FilterInfo filterInfo(const string_ref &name) const;

    // devices //

    bool deviceOptions(uint64_t hw_id, const string_ref &device_options);

    vector<DeviceInfo> findDeviceInfo(const string_ref &type, const string_ref &name=string_ref()) const;

    DeviceInfo deviceInfo(uint64_t hw_id) const;

    // pack ///

    bool pack(const PackInfo *first, const PackInfo *last);

    vector<PackInfo> packInfo() const;

    // events //

    const Event* peekEvent(long timeout=0);
    const Event* nextEvent(long timeout=0);

    // property //

    Variant property(const string_ref &name) const;
    template <typename T> T property(const string_ref &name) const;

  protected:
    ContextData *data;
    Context(const Context &ctx); // disable copy constructor
  };

  //// Type ////

  struct OWLAPI Type {

    enum {
      INVALID = 0, BYTE, STRING = BYTE, INT, FLOAT,
      ERROR = 0x7F,
      EVENT = 0x80, FRAME = EVENT, CAMERA, PEAK, PLANE,
      MARKER, RIGID, INPUT,
      MARKERINFO, TRACKERINFO, FILTERINFO, DEVICEINFO, PACKINFO
    };

    Type(uint32_t id, const void *data);

    template <typename T> operator const T*() const;
    template <typename T> operator T() const;

    template <typename T> struct ID {
      bool operator==(uint32_t id) const;
    };

  protected:
    const uint32_t id;
    const void * const data;
  };

  //// Variant ////

  struct OWLAPI Variant {

    Variant();
    Variant(const Variant &v);
    ~Variant();

    Variant& operator=(const Variant &v);

    uint16_t type_id() const;
    uint32_t flags() const;
    const char* type_name() const;

    bool valid() const;
    bool empty() const;

    Type begin() const;
    Type end() const;

    template <typename T> operator T() const;
    template <typename T> operator std::vector<T>() const;

    template <typename T> size_t get(T &v) const;

    string str() const;

  protected:
    uint32_t _id;
    uint32_t _flags;
    void *_data, *_data_end;
    const char *_type_name;
  };

  //// Event ////

  struct OWLAPI Event : public Variant {

    Event();

    uint16_t type_id() const;
    uint16_t id() const;
    uint32_t flags() const;
    int64_t time() const;
    const char* type_name() const;
    const char* name() const;

    template <typename T> size_t size() const;

    string str() const;

    const Event* find(uint16_t type_id, const string_ref &name) const;
    const Event* find(const string_ref &name) const;

    template <typename T> size_t find(const string_ref &name, T &v) const;

  protected:
    const char *_name;
    int64_t _time;

    friend class ContextData;
  };

  //// Scanner ////

  class OWLAPI Scan {
    int fd;
  public:

    Scan();
    ~Scan();

    bool send(const string_ref &message);
    vector<string> listen(long timeout=0);
  };

  //// Context ////

  template <typename T> T Context::property(const string_ref &name) const
  { return property(name); }

  //// Type ////

  template <typename T> Type::operator const T*() const
  { return ID<T>() == id ? (const T*)data : 0;  }

  template <typename T> Type::operator T() const
  { return ID<T>() == id && (const T*)data ? *(const T*)data : T();  }

  template <typename T> bool Type::ID<T>::operator==(uint32_t id) const { return false; }
  template <> inline bool Type::ID<void>::operator==(uint32_t id) const { return true; }
  template <> inline bool Type::ID<char>::operator==(uint32_t id) const { return id == BYTE || id == ERROR; }
  template <> inline bool Type::ID<int>::operator==(uint32_t id) const { return id == INT; }
  template <> inline bool Type::ID<unsigned int>::operator==(uint32_t id) const { return id == INT; }
  template <> inline bool Type::ID<float>::operator==(uint32_t id) const { return id == FLOAT; }
  template <> inline bool Type::ID<Event>::operator==(uint32_t id) const { return id == EVENT; }
  template <> inline bool Type::ID<Camera>::operator==(uint32_t id) const { return id == CAMERA; }
  template <> inline bool Type::ID<Peak>::operator==(uint32_t id) const { return id == PEAK; }
  template <> inline bool Type::ID<Plane>::operator==(uint32_t id) const { return id == PLANE; }
  template <> inline bool Type::ID<Marker>::operator==(uint32_t id) const { return id == MARKER; }
  template <> inline bool Type::ID<Rigid>::operator==(uint32_t id) const { return id == RIGID; }
  template <> inline bool Type::ID<Input>::operator==(uint32_t id) const { return id == INPUT; }
  template <> inline bool Type::ID<MarkerInfo>::operator==(uint32_t id) const { return id == MARKERINFO; }
  template <> inline bool Type::ID<TrackerInfo>::operator==(uint32_t id) const { return id == TRACKERINFO; }
  template <> inline bool Type::ID<FilterInfo>::operator==(uint32_t id) const { return id == FILTERINFO; }
  template <> inline bool Type::ID<DeviceInfo>::operator==(uint32_t id) const { return id == DEVICEINFO; }
  template <> inline bool Type::ID<PackInfo>::operator==(uint32_t id) const { return id == PACKINFO; }

  //// Variant ////

  template <typename T> Variant::operator T() const { return begin(); }

  template <> inline Variant::operator std::string() const
  { return std::string((const char*)begin(), (const char*)end()); }

  template <typename T> Variant::operator std::vector<T>() const
  { return std::vector<T>((const T*)begin(), (const T*)end()); }

  template <typename T> size_t Variant::get(T &v) const
  {
    return Type::ID<typename T::value_type>() == type_id() ?
      (v = T((typename T::const_pointer)begin(), (typename T::const_pointer)end())).size() : 0;
  }

  //// Event ////

  template <typename T> size_t Event::size() const
  { return Type::ID<T>() == type_id() ? ((const char*)_data_end - (const char*)_data) / sizeof(T) : 0; }

  template <typename T> size_t Event::find(const string_ref &name, T &v) const
  {
    if(type_id() == Type::FRAME)
      for(const Event *e = begin(); e != end(); e++)
        if(Type::ID<typename T::value_type>() == e->type_id() && name == e->name())
          return (v = T((typename T::const_pointer)e->begin(), (typename T::const_pointer)e->end())).size();
    return 0;
  }

  //// conversion specializations ////

#define OWL_TYPE_OPERATORS(T1, T2)                                      \
  template <> inline Type::operator T1() const                          \
  { return ID<T1>() == id && (const T1*)data ? *(const T1*)data : ID<T2>() == id && (const T2*)data ? (T1)*(const T2*)data : T1(); }

#define OWL_VARIANT_VECTOR_OPERATORS(V, T1, T2)                         \
  template <> inline Variant::operator V<T1>() const                    \
  {                                                                     \
    if(Type::ID<T2>() == type_id())                                     \
      {                                                                 \
        V<T1> out;                                                      \
        for(const T2* i = (const T2*)begin(); i != (const T2*)end(); i++) \
          out.push_back((T1)*i);                                        \
        return out;                                                     \
      }                                                                 \
    return V<T1>((const T1*)begin(), (const T1*)end());                 \
  }

  OWL_TYPE_OPERATORS(float, int);
  OWL_TYPE_OPERATORS(int, float);

  OWL_VARIANT_VECTOR_OPERATORS(std::vector, float, int);
  OWL_VARIANT_VECTOR_OPERATORS(std::vector, int, float);

  ////

} // namespace OWL

////

#endif // OWL_HPP
