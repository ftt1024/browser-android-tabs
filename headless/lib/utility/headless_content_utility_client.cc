// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "headless/lib/utility/headless_content_utility_client.h"

#include "base/lazy_instance.h"
#include "printing/buildflags/buildflags.h"

#if BUILDFLAG(ENABLE_PRINTING)
#include "components/services/pdf_compositor/public/cpp/pdf_compositor_service_factory.h"
#include "components/services/pdf_compositor/public/interfaces/pdf_compositor.mojom.h"
#endif

namespace headless {

namespace {
base::LazyInstance<
    HeadlessContentUtilityClient::NetworkBinderCreationCallback>::Leaky
    g_network_binder_creation_callback = LAZY_INSTANCE_INITIALIZER;
};

// static
void HeadlessContentUtilityClient::SetNetworkBinderCreationCallbackForTests(
    NetworkBinderCreationCallback callback) {
  g_network_binder_creation_callback.Get() = std::move(callback);
}

HeadlessContentUtilityClient::HeadlessContentUtilityClient(
    const std::string& user_agent)
    : user_agent_(user_agent) {}

HeadlessContentUtilityClient::~HeadlessContentUtilityClient() = default;

std::unique_ptr<service_manager::Service>
HeadlessContentUtilityClient::HandleServiceRequest(
    const std::string& service_name,
    service_manager::mojom::ServiceRequest request) {
#if BUILDFLAG(ENABLE_PRINTING) && !defined(CHROME_MULTIPLE_DLL_BROWSER)
  if (service_name == printing::mojom::kServiceName) {
    return printing::CreatePdfCompositorService(user_agent_,
                                                std::move(request));
  }
#endif

  return nullptr;
}

void HeadlessContentUtilityClient::RegisterNetworkBinders(
    service_manager::BinderRegistry* registry) {
  if (g_network_binder_creation_callback.Get())
    g_network_binder_creation_callback.Get().Run(registry);
}

}  // namespace headless
