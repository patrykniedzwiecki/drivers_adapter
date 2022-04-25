/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HDI_SAMPLE_CLIENT_PROXY_CPP_INF_H
#define HDI_SAMPLE_CLIENT_PROXY_CPP_INF_H

#include <iproxy_broker.h>

#include "isample.h"

namespace OHOS {
namespace HDI {
namespace Sample {
namespace V1_0 {
class SampleProxy : public IProxyBroker<ISample> {
public:
    explicit SampleProxy(const sptr<IRemoteObject> &impl) : IProxyBroker<ISample>(impl) {}
    virtual ~SampleProxy() = default;

    int32_t GetInterface(sptr<IFoo> &output) override;

private:
    static inline BrokerDelegator<SampleProxy> delegator_;
};
} // namespace V1_0
} // namespace Sample
} // namespace HDI
} // namespace OHOS

#endif // HDI_SAMPLE_CLIENT_PROXY_CPP_INF_H