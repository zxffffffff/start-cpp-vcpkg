// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Res.proto
// Protobuf C++ Version: 4.25.1

#ifndef GOOGLE_PROTOBUF_INCLUDED_Res_2eproto_2epb_2eh
#define GOOGLE_PROTOBUF_INCLUDED_Res_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>
#include <utility>

#include "google/protobuf/port_def.inc"
#if PROTOBUF_VERSION < 4025000
#error "This file was generated by a newer version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please update"
#error "your headers."
#endif  // PROTOBUF_VERSION

#if 4025001 < PROTOBUF_MIN_PROTOC_VERSION
#error "This file was generated by an older version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please"
#error "regenerate this file with a newer version of protoc."
#endif  // PROTOBUF_MIN_PROTOC_VERSION
#include "google/protobuf/port_undef.inc"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_tctable_decl.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/message.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "google/protobuf/unknown_field_set.h"
#include "google/protobuf/any.pb.h"
#include "MsgType.pb.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_Res_2eproto

namespace google {
namespace protobuf {
namespace internal {
class AnyMetadata;
}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Internal implementation detail -- do not use these members.
struct TableStruct_Res_2eproto {
  static const ::uint32_t offsets[];
};
extern const ::google::protobuf::internal::DescriptorTable
    descriptor_table_Res_2eproto;
class PBRes;
struct PBResDefaultTypeInternal;
extern PBResDefaultTypeInternal _PBRes_default_instance_;
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google


// ===================================================================


// -------------------------------------------------------------------

class PBRes final :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:PBRes) */ {
 public:
  inline PBRes() : PBRes(nullptr) {}
  ~PBRes() override;
  template<typename = void>
  explicit PROTOBUF_CONSTEXPR PBRes(::google::protobuf::internal::ConstantInitialized);

  inline PBRes(const PBRes& from)
      : PBRes(nullptr, from) {}
  PBRes(PBRes&& from) noexcept
    : PBRes() {
    *this = ::std::move(from);
  }

  inline PBRes& operator=(const PBRes& from) {
    CopyFrom(from);
    return *this;
  }
  inline PBRes& operator=(PBRes&& from) noexcept {
    if (this == &from) return *this;
    if (GetArena() == from.GetArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance);
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields()
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.mutable_unknown_fields<::google::protobuf::UnknownFieldSet>();
  }

  static const ::google::protobuf::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::google::protobuf::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::google::protobuf::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const PBRes& default_instance() {
    return *internal_default_instance();
  }
  static inline const PBRes* internal_default_instance() {
    return reinterpret_cast<const PBRes*>(
               &_PBRes_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(PBRes& a, PBRes& b) {
    a.Swap(&b);
  }
  inline void Swap(PBRes* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetArena() != nullptr &&
        GetArena() == other->GetArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetArena() == other->GetArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::google::protobuf::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(PBRes* other) {
    if (other == this) return;
    ABSL_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  PBRes* New(::google::protobuf::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<PBRes>(arena);
  }
  using ::google::protobuf::Message::CopyFrom;
  void CopyFrom(const PBRes& from);
  using ::google::protobuf::Message::MergeFrom;
  void MergeFrom( const PBRes& from) {
    PBRes::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  ::size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::google::protobuf::internal::ParseContext* ctx) final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const { return _impl_._cached_size_.Get(); }

  private:
  ::google::protobuf::internal::CachedSize* AccessCachedSize() const final;
  void SharedCtor(::google::protobuf::Arena* arena);
  void SharedDtor();
  void InternalSwap(PBRes* other);

  private:
  friend class ::google::protobuf::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "PBRes";
  }
  protected:
  explicit PBRes(::google::protobuf::Arena* arena);
  PBRes(::google::protobuf::Arena* arena, const PBRes& from);
  public:

  static const ClassData _class_data_;
  const ::google::protobuf::Message::ClassData*GetClassData() const final;

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kReqIdFieldNumber = 2,
    kRetMsgFieldNumber = 5,
    kPayloadFieldNumber = 6,
    kResMsgTypeFieldNumber = 1,
    kRetCodeFieldNumber = 4,
    kResTimeFieldNumber = 3,
  };
  // string reqId = 2;
  void clear_reqid() ;
  const std::string& reqid() const;
  template <typename Arg_ = const std::string&, typename... Args_>
  void set_reqid(Arg_&& arg, Args_... args);
  std::string* mutable_reqid();
  PROTOBUF_NODISCARD std::string* release_reqid();
  void set_allocated_reqid(std::string* value);

  private:
  const std::string& _internal_reqid() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_reqid(
      const std::string& value);
  std::string* _internal_mutable_reqid();

  public:
  // string retMsg = 5;
  void clear_retmsg() ;
  const std::string& retmsg() const;
  template <typename Arg_ = const std::string&, typename... Args_>
  void set_retmsg(Arg_&& arg, Args_... args);
  std::string* mutable_retmsg();
  PROTOBUF_NODISCARD std::string* release_retmsg();
  void set_allocated_retmsg(std::string* value);

  private:
  const std::string& _internal_retmsg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_retmsg(
      const std::string& value);
  std::string* _internal_mutable_retmsg();

  public:
  // .google.protobuf.Any payload = 6;
  bool has_payload() const;
  void clear_payload() ;
  const ::google::protobuf::Any& payload() const;
  PROTOBUF_NODISCARD ::google::protobuf::Any* release_payload();
  ::google::protobuf::Any* mutable_payload();
  void set_allocated_payload(::google::protobuf::Any* value);
  void unsafe_arena_set_allocated_payload(::google::protobuf::Any* value);
  ::google::protobuf::Any* unsafe_arena_release_payload();

  private:
  const ::google::protobuf::Any& _internal_payload() const;
  ::google::protobuf::Any* _internal_mutable_payload();

  public:
  // .PBMsgType resMsgType = 1;
  void clear_resmsgtype() ;
  ::PBMsgType resmsgtype() const;
  void set_resmsgtype(::PBMsgType value);

  private:
  ::PBMsgType _internal_resmsgtype() const;
  void _internal_set_resmsgtype(::PBMsgType value);

  public:
  // int32 retCode = 4;
  void clear_retcode() ;
  ::int32_t retcode() const;
  void set_retcode(::int32_t value);

  private:
  ::int32_t _internal_retcode() const;
  void _internal_set_retcode(::int32_t value);

  public:
  // uint64 resTime = 3;
  void clear_restime() ;
  ::uint64_t restime() const;
  void set_restime(::uint64_t value);

  private:
  ::uint64_t _internal_restime() const;
  void _internal_set_restime(::uint64_t value);

  public:
  // @@protoc_insertion_point(class_scope:PBRes)
 private:
  class _Internal;

  friend class ::google::protobuf::internal::TcParser;
  static const ::google::protobuf::internal::TcParseTable<
      3, 6, 1,
      25, 2>
      _table_;
  friend class ::google::protobuf::MessageLite;
  friend class ::google::protobuf::Arena;
  template <typename T>
  friend class ::google::protobuf::Arena::InternalHelper;
  using InternalArenaConstructable_ = void;
  using DestructorSkippable_ = void;
  struct Impl_ {

        inline explicit constexpr Impl_(
            ::google::protobuf::internal::ConstantInitialized) noexcept;
        inline explicit Impl_(::google::protobuf::internal::InternalVisibility visibility,
                              ::google::protobuf::Arena* arena);
        inline explicit Impl_(::google::protobuf::internal::InternalVisibility visibility,
                              ::google::protobuf::Arena* arena, const Impl_& from);
    ::google::protobuf::internal::HasBits<1> _has_bits_;
    mutable ::google::protobuf::internal::CachedSize _cached_size_;
    ::google::protobuf::internal::ArenaStringPtr reqid_;
    ::google::protobuf::internal::ArenaStringPtr retmsg_;
    ::google::protobuf::Any* payload_;
    int resmsgtype_;
    ::int32_t retcode_;
    ::uint64_t restime_;
    PROTOBUF_TSAN_DECLARE_MEMBER
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_Res_2eproto;
};

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// PBRes

// .PBMsgType resMsgType = 1;
inline void PBRes::clear_resmsgtype() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.resmsgtype_ = 0;
}
inline ::PBMsgType PBRes::resmsgtype() const {
  // @@protoc_insertion_point(field_get:PBRes.resMsgType)
  return _internal_resmsgtype();
}
inline void PBRes::set_resmsgtype(::PBMsgType value) {
  _internal_set_resmsgtype(value);
  // @@protoc_insertion_point(field_set:PBRes.resMsgType)
}
inline ::PBMsgType PBRes::_internal_resmsgtype() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return static_cast<::PBMsgType>(_impl_.resmsgtype_);
}
inline void PBRes::_internal_set_resmsgtype(::PBMsgType value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.resmsgtype_ = value;
}

// string reqId = 2;
inline void PBRes::clear_reqid() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.reqid_.ClearToEmpty();
}
inline const std::string& PBRes::reqid() const
    ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:PBRes.reqId)
  return _internal_reqid();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void PBRes::set_reqid(Arg_&& arg,
                                                     Args_... args) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.reqid_.Set(static_cast<Arg_&&>(arg), args..., GetArena());
  // @@protoc_insertion_point(field_set:PBRes.reqId)
}
inline std::string* PBRes::mutable_reqid() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  std::string* _s = _internal_mutable_reqid();
  // @@protoc_insertion_point(field_mutable:PBRes.reqId)
  return _s;
}
inline const std::string& PBRes::_internal_reqid() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.reqid_.Get();
}
inline void PBRes::_internal_set_reqid(const std::string& value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.reqid_.Set(value, GetArena());
}
inline std::string* PBRes::_internal_mutable_reqid() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  return _impl_.reqid_.Mutable( GetArena());
}
inline std::string* PBRes::release_reqid() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  // @@protoc_insertion_point(field_release:PBRes.reqId)
  return _impl_.reqid_.Release();
}
inline void PBRes::set_allocated_reqid(std::string* value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.reqid_.SetAllocated(value, GetArena());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.reqid_.IsDefault()) {
          _impl_.reqid_.Set("", GetArena());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:PBRes.reqId)
}

// uint64 resTime = 3;
inline void PBRes::clear_restime() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.restime_ = ::uint64_t{0u};
}
inline ::uint64_t PBRes::restime() const {
  // @@protoc_insertion_point(field_get:PBRes.resTime)
  return _internal_restime();
}
inline void PBRes::set_restime(::uint64_t value) {
  _internal_set_restime(value);
  // @@protoc_insertion_point(field_set:PBRes.resTime)
}
inline ::uint64_t PBRes::_internal_restime() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.restime_;
}
inline void PBRes::_internal_set_restime(::uint64_t value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.restime_ = value;
}

// int32 retCode = 4;
inline void PBRes::clear_retcode() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.retcode_ = 0;
}
inline ::int32_t PBRes::retcode() const {
  // @@protoc_insertion_point(field_get:PBRes.retCode)
  return _internal_retcode();
}
inline void PBRes::set_retcode(::int32_t value) {
  _internal_set_retcode(value);
  // @@protoc_insertion_point(field_set:PBRes.retCode)
}
inline ::int32_t PBRes::_internal_retcode() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.retcode_;
}
inline void PBRes::_internal_set_retcode(::int32_t value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.retcode_ = value;
}

// string retMsg = 5;
inline void PBRes::clear_retmsg() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.retmsg_.ClearToEmpty();
}
inline const std::string& PBRes::retmsg() const
    ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:PBRes.retMsg)
  return _internal_retmsg();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void PBRes::set_retmsg(Arg_&& arg,
                                                     Args_... args) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.retmsg_.Set(static_cast<Arg_&&>(arg), args..., GetArena());
  // @@protoc_insertion_point(field_set:PBRes.retMsg)
}
inline std::string* PBRes::mutable_retmsg() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  std::string* _s = _internal_mutable_retmsg();
  // @@protoc_insertion_point(field_mutable:PBRes.retMsg)
  return _s;
}
inline const std::string& PBRes::_internal_retmsg() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.retmsg_.Get();
}
inline void PBRes::_internal_set_retmsg(const std::string& value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.retmsg_.Set(value, GetArena());
}
inline std::string* PBRes::_internal_mutable_retmsg() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  return _impl_.retmsg_.Mutable( GetArena());
}
inline std::string* PBRes::release_retmsg() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  // @@protoc_insertion_point(field_release:PBRes.retMsg)
  return _impl_.retmsg_.Release();
}
inline void PBRes::set_allocated_retmsg(std::string* value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.retmsg_.SetAllocated(value, GetArena());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.retmsg_.IsDefault()) {
          _impl_.retmsg_.Set("", GetArena());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:PBRes.retMsg)
}

// .google.protobuf.Any payload = 6;
inline bool PBRes::has_payload() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.payload_ != nullptr);
  return value;
}
inline const ::google::protobuf::Any& PBRes::_internal_payload() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  const ::google::protobuf::Any* p = _impl_.payload_;
  return p != nullptr ? *p : reinterpret_cast<const ::google::protobuf::Any&>(::google::protobuf::_Any_default_instance_);
}
inline const ::google::protobuf::Any& PBRes::payload() const ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:PBRes.payload)
  return _internal_payload();
}
inline void PBRes::unsafe_arena_set_allocated_payload(::google::protobuf::Any* value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  if (GetArena() == nullptr) {
    delete reinterpret_cast<::google::protobuf::MessageLite*>(_impl_.payload_);
  }
  _impl_.payload_ = reinterpret_cast<::google::protobuf::Any*>(value);
  if (value != nullptr) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:PBRes.payload)
}
inline ::google::protobuf::Any* PBRes::release_payload() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);

  _impl_._has_bits_[0] &= ~0x00000001u;
  ::google::protobuf::Any* released = _impl_.payload_;
  _impl_.payload_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old = reinterpret_cast<::google::protobuf::MessageLite*>(released);
  released = ::google::protobuf::internal::DuplicateIfNonNull(released);
  if (GetArena() == nullptr) {
    delete old;
  }
#else   // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArena() != nullptr) {
    released = ::google::protobuf::internal::DuplicateIfNonNull(released);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return released;
}
inline ::google::protobuf::Any* PBRes::unsafe_arena_release_payload() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  // @@protoc_insertion_point(field_release:PBRes.payload)

  _impl_._has_bits_[0] &= ~0x00000001u;
  ::google::protobuf::Any* temp = _impl_.payload_;
  _impl_.payload_ = nullptr;
  return temp;
}
inline ::google::protobuf::Any* PBRes::_internal_mutable_payload() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_._has_bits_[0] |= 0x00000001u;
  if (_impl_.payload_ == nullptr) {
    auto* p = CreateMaybeMessage<::google::protobuf::Any>(GetArena());
    _impl_.payload_ = reinterpret_cast<::google::protobuf::Any*>(p);
  }
  return _impl_.payload_;
}
inline ::google::protobuf::Any* PBRes::mutable_payload() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  ::google::protobuf::Any* _msg = _internal_mutable_payload();
  // @@protoc_insertion_point(field_mutable:PBRes.payload)
  return _msg;
}
inline void PBRes::set_allocated_payload(::google::protobuf::Any* value) {
  ::google::protobuf::Arena* message_arena = GetArena();
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  if (message_arena == nullptr) {
    delete reinterpret_cast<::google::protobuf::MessageLite*>(_impl_.payload_);
  }

  if (value != nullptr) {
    ::google::protobuf::Arena* submessage_arena = reinterpret_cast<::google::protobuf::MessageLite*>(value)->GetArena();
    if (message_arena != submessage_arena) {
      value = ::google::protobuf::internal::GetOwnedMessage(message_arena, value, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }

  _impl_.payload_ = reinterpret_cast<::google::protobuf::Any*>(value);
  // @@protoc_insertion_point(field_set_allocated:PBRes.payload)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INCLUDED_Res_2eproto_2epb_2eh
