#include "SamplePB.h"
#include "google/protobuf/stubs/common.h"
#include "Req.pb.h"
#include "Res.pb.h"

SamplePB::SamplePB()
{
    std::cout << "Hello SamplePB." << std::endl;

    std::cout << "protobuf version: " << google::protobuf::internal::VersionString(GOOGLE_PROTOBUF_VERSION) << std::endl;
    {
        PBReq req;
        req.set_reqid("abc");
        req.set_reqtime(123);
        auto raw = req.SerializeAsString();

        PBReq req2;
        req2.ParseFromString(raw);
        assert(req2.reqid() == req.reqid());
        assert(req2.reqtime() == req.reqtime());
    }
}
