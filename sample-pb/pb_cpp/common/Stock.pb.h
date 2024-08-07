// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common/Stock.proto
// Protobuf C++ Version: 4.25.1

#ifndef GOOGLE_PROTOBUF_INCLUDED_common_2fStock_2eproto_2epb_2eh
#define GOOGLE_PROTOBUF_INCLUDED_common_2fStock_2eproto_2epb_2eh

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
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_common_2fStock_2eproto

namespace google {
namespace protobuf {
namespace internal {
class AnyMetadata;
}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Internal implementation detail -- do not use these members.
struct TableStruct_common_2fStock_2eproto {
  static const ::uint32_t offsets[];
};
extern const ::google::protobuf::internal::DescriptorTable
    descriptor_table_common_2fStock_2eproto;
class Stock;
struct StockDefaultTypeInternal;
extern StockDefaultTypeInternal _Stock_default_instance_;
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google


// ===================================================================


// -------------------------------------------------------------------

class Stock final :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Stock) */ {
 public:
  inline Stock() : Stock(nullptr) {}
  ~Stock() override;
  template<typename = void>
  explicit PROTOBUF_CONSTEXPR Stock(::google::protobuf::internal::ConstantInitialized);

  inline Stock(const Stock& from)
      : Stock(nullptr, from) {}
  Stock(Stock&& from) noexcept
    : Stock() {
    *this = ::std::move(from);
  }

  inline Stock& operator=(const Stock& from) {
    CopyFrom(from);
    return *this;
  }
  inline Stock& operator=(Stock&& from) noexcept {
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
  static const Stock& default_instance() {
    return *internal_default_instance();
  }
  static inline const Stock* internal_default_instance() {
    return reinterpret_cast<const Stock*>(
               &_Stock_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Stock& a, Stock& b) {
    a.Swap(&b);
  }
  inline void Swap(Stock* other) {
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
  void UnsafeArenaSwap(Stock* other) {
    if (other == this) return;
    ABSL_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Stock* New(::google::protobuf::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Stock>(arena);
  }
  using ::google::protobuf::Message::CopyFrom;
  void CopyFrom(const Stock& from);
  using ::google::protobuf::Message::MergeFrom;
  void MergeFrom( const Stock& from) {
    Stock::MergeImpl(*this, from);
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
  void InternalSwap(Stock* other);

  private:
  friend class ::google::protobuf::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "Stock";
  }
  protected:
  explicit Stock(::google::protobuf::Arena* arena);
  Stock(::google::protobuf::Arena* arena, const Stock& from);
  public:

  static const ClassData _class_data_;
  const ::google::protobuf::Message::ClassData*GetClassData() const final;

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kCodeFieldNumber = 1,
    kMarketFieldNumber = 2,
  };
  // string code = 1;
  void clear_code() ;
  const std::string& code() const;
  template <typename Arg_ = const std::string&, typename... Args_>
  void set_code(Arg_&& arg, Args_... args);
  std::string* mutable_code();
  PROTOBUF_NODISCARD std::string* release_code();
  void set_allocated_code(std::string* value);

  private:
  const std::string& _internal_code() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_code(
      const std::string& value);
  std::string* _internal_mutable_code();

  public:
  // string market = 2;
  void clear_market() ;
  const std::string& market() const;
  template <typename Arg_ = const std::string&, typename... Args_>
  void set_market(Arg_&& arg, Args_... args);
  std::string* mutable_market();
  PROTOBUF_NODISCARD std::string* release_market();
  void set_allocated_market(std::string* value);

  private:
  const std::string& _internal_market() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_market(
      const std::string& value);
  std::string* _internal_mutable_market();

  public:
  // @@protoc_insertion_point(class_scope:Stock)
 private:
  class _Internal;

  friend class ::google::protobuf::internal::TcParser;
  static const ::google::protobuf::internal::TcParseTable<
      1, 2, 0,
      24, 2>
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
    ::google::protobuf::internal::ArenaStringPtr code_;
    ::google::protobuf::internal::ArenaStringPtr market_;
    mutable ::google::protobuf::internal::CachedSize _cached_size_;
    PROTOBUF_TSAN_DECLARE_MEMBER
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_common_2fStock_2eproto;
};

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// Stock

// string code = 1;
inline void Stock::clear_code() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.code_.ClearToEmpty();
}
inline const std::string& Stock::code() const
    ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:Stock.code)
  return _internal_code();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void Stock::set_code(Arg_&& arg,
                                                     Args_... args) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.code_.Set(static_cast<Arg_&&>(arg), args..., GetArena());
  // @@protoc_insertion_point(field_set:Stock.code)
}
inline std::string* Stock::mutable_code() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  std::string* _s = _internal_mutable_code();
  // @@protoc_insertion_point(field_mutable:Stock.code)
  return _s;
}
inline const std::string& Stock::_internal_code() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.code_.Get();
}
inline void Stock::_internal_set_code(const std::string& value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.code_.Set(value, GetArena());
}
inline std::string* Stock::_internal_mutable_code() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  return _impl_.code_.Mutable( GetArena());
}
inline std::string* Stock::release_code() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  // @@protoc_insertion_point(field_release:Stock.code)
  return _impl_.code_.Release();
}
inline void Stock::set_allocated_code(std::string* value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.code_.SetAllocated(value, GetArena());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.code_.IsDefault()) {
          _impl_.code_.Set("", GetArena());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Stock.code)
}

// string market = 2;
inline void Stock::clear_market() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.market_.ClearToEmpty();
}
inline const std::string& Stock::market() const
    ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:Stock.market)
  return _internal_market();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void Stock::set_market(Arg_&& arg,
                                                     Args_... args) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.market_.Set(static_cast<Arg_&&>(arg), args..., GetArena());
  // @@protoc_insertion_point(field_set:Stock.market)
}
inline std::string* Stock::mutable_market() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  std::string* _s = _internal_mutable_market();
  // @@protoc_insertion_point(field_mutable:Stock.market)
  return _s;
}
inline const std::string& Stock::_internal_market() const {
  PROTOBUF_TSAN_READ(&_impl_._tsan_detect_race);
  return _impl_.market_.Get();
}
inline void Stock::_internal_set_market(const std::string& value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  _impl_.market_.Set(value, GetArena());
}
inline std::string* Stock::_internal_mutable_market() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ;
  return _impl_.market_.Mutable( GetArena());
}
inline std::string* Stock::release_market() {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  // @@protoc_insertion_point(field_release:Stock.market)
  return _impl_.market_.Release();
}
inline void Stock::set_allocated_market(std::string* value) {
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  _impl_.market_.SetAllocated(value, GetArena());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.market_.IsDefault()) {
          _impl_.market_.Set("", GetArena());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Stock.market)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INCLUDED_common_2fStock_2eproto_2epb_2eh
