// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: req/HeartbeatReq.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_req_2fHeartbeatReq_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_req_2fHeartbeatReq_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_req_2fHeartbeatReq_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_req_2fHeartbeatReq_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_req_2fHeartbeatReq_2eproto;
class HeartbeatReq;
struct HeartbeatReqDefaultTypeInternal;
extern HeartbeatReqDefaultTypeInternal _HeartbeatReq_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::HeartbeatReq* Arena::CreateMaybeMessage<::HeartbeatReq>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class HeartbeatReq final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:HeartbeatReq) */ {
 public:
  inline HeartbeatReq() : HeartbeatReq(nullptr) {}
  ~HeartbeatReq() override;
  explicit PROTOBUF_CONSTEXPR HeartbeatReq(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  HeartbeatReq(const HeartbeatReq& from);
  HeartbeatReq(HeartbeatReq&& from) noexcept
    : HeartbeatReq() {
    *this = ::std::move(from);
  }

  inline HeartbeatReq& operator=(const HeartbeatReq& from) {
    CopyFrom(from);
    return *this;
  }
  inline HeartbeatReq& operator=(HeartbeatReq&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const HeartbeatReq& default_instance() {
    return *internal_default_instance();
  }
  static inline const HeartbeatReq* internal_default_instance() {
    return reinterpret_cast<const HeartbeatReq*>(
               &_HeartbeatReq_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(HeartbeatReq& a, HeartbeatReq& b) {
    a.Swap(&b);
  }
  inline void Swap(HeartbeatReq* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(HeartbeatReq* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  HeartbeatReq* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<HeartbeatReq>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const HeartbeatReq& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const HeartbeatReq& from) {
    HeartbeatReq::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(HeartbeatReq* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "HeartbeatReq";
  }
  protected:
  explicit HeartbeatReq(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kReqIndexFieldNumber = 1,
  };
  // int64 reqIndex = 1;
  void clear_reqindex();
  int64_t reqindex() const;
  void set_reqindex(int64_t value);
  private:
  int64_t _internal_reqindex() const;
  void _internal_set_reqindex(int64_t value);
  public:

  // @@protoc_insertion_point(class_scope:HeartbeatReq)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    int64_t reqindex_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_req_2fHeartbeatReq_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// HeartbeatReq

// int64 reqIndex = 1;
inline void HeartbeatReq::clear_reqindex() {
  _impl_.reqindex_ = int64_t{0};
}
inline int64_t HeartbeatReq::_internal_reqindex() const {
  return _impl_.reqindex_;
}
inline int64_t HeartbeatReq::reqindex() const {
  // @@protoc_insertion_point(field_get:HeartbeatReq.reqIndex)
  return _internal_reqindex();
}
inline void HeartbeatReq::_internal_set_reqindex(int64_t value) {
  
  _impl_.reqindex_ = value;
}
inline void HeartbeatReq::set_reqindex(int64_t value) {
  _internal_set_reqindex(value);
  // @@protoc_insertion_point(field_set:HeartbeatReq.reqIndex)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_req_2fHeartbeatReq_2eproto
