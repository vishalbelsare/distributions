// Copyright (c) 2014, Salesforce.com, Inc.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// - Neither the name of Salesforce.com nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <distributions/common.hpp>
#include <distributions/schema.pb.h>
#include <distributions/clustering.hpp>
#include <distributions/models/dd.hpp>
#include <distributions/models/dpd.hpp>
#include <distributions/models/nich.hpp>
#include <distributions/models/gp.hpp>

namespace distributions
{

namespace protobuf { using namespace ::protobuf::distributions; }

//----------------------------------------------------------------------------
// Clustering

// FIXME g++ fails if these are made template<count_t>

inline void clustering_load (
        typename Clustering<int>::PitmanYor & model,
        const protobuf::Clustering::PitmanYor & message)
{
    model.alpha = message.alpha();
    model.d = message.d();
}

inline void clustering_load (
        typename Clustering<int>::PitmanYor & model,
        const protobuf::Clustering & message)
{
    clustering_load(model, message.pitman_yor());
}

inline void clustering_load (
        typename Clustering<int>::LowEntropy & model,
        const protobuf::Clustering::LowEntropy & message)
{
    model.dataset_size = message.dataset_size();
}

template<class count_t>
inline void clustering_load (
        typename Clustering<int>::LowEntropy & model,
        const protobuf::Clustering & message)
{
    clustering_load(model, message.low_entropy());
}

//----------------------------------------------------------------------------
// Models

template<int max_dim>
inline void model_load (
        DirichletDiscrete<max_dim> & model,
        const protobuf::DirichletDiscrete & message)
{
    model.dim = message.alphas_size();
    DIST_ASSERT(model.dim <= 16, "dim is too large: " << model.dim);
    for (size_t i = 0; i < model.dim; ++i) {
        model.alphas[i] = message.alphas(i);
    }
}

inline void model_load (
        DirichletProcessDiscrete & model,
        const protobuf::DirichletProcessDiscrete & message)
{
    model.gamma = message.gamma();
    model.alpha = message.alpha();
    model.betas.resize(message.betas_size());
    double beta_sum = 0;
    for (size_t i = 0; i < model.betas.size(); ++i) {
        beta_sum += model.betas[i] = message.betas(i);
    }
    model.beta0 = 1 - beta_sum;
}

inline void model_load (
        GammaPoisson & model,
        const protobuf::GammaPoisson & message)
{
    model.alpha = message.alpha();
    model.inv_beta = message.inv_beta();
}

inline void model_load (
        NormalInverseChiSq & model,
        const protobuf::NormalInverseChiSq & message)
{
    model.mu = message.mu();
    model.kappa = message.kappa();
    model.sigmasq = message.sigmasq();
    model.nu = message.nu();
}

//----------------------------------------------------------------------------
// Groups

template<int max_dim>
inline void group_dump (
        const DirichletDiscrete<max_dim> & model,
        const typename DirichletDiscrete<max_dim>::Group & group,
        protobuf::DirichletDiscrete::Group & message)
{
    message.Clear();
    auto & counts = * message.mutable_counts();
    for (size_t i = 0; i < model.dim; ++i) {
        counts.Add(group.counts[i]);
    }
}

inline void group_dump (
        const DirichletProcessDiscrete &,
        const DirichletProcessDiscrete::Group & group,
        protobuf::DirichletProcessDiscrete::Group & message)
{
    message.Clear();
    auto & keys = * message.mutable_keys();
    auto & values = * message.mutable_values();
    for (const auto & pair : group.counts) {
        keys.Add(pair.first);
        values.Add(pair.second);
    }
}

inline void group_dump (
        const GammaPoisson &,
        const GammaPoisson::Group & group,
        protobuf::GammaPoisson::Group & message)
{
    message.set_count(group.count);
    message.set_sum(group.sum);
    message.set_log_prod(group.log_prod);
}

inline void group_dump (
        const NormalInverseChiSq &,
        const NormalInverseChiSq::Group & group,
        protobuf::NormalInverseChiSq::Group & message)
{
    message.set_count(group.count);
    message.set_mean(group.mean);
    message.set_count_times_variance(group.count_times_variance);
}

} // namespace distributions