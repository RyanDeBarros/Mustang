#include "CHRS.h"

#include <glm/ext/scalar_constants.hpp>

#include "render/actors/particles/ParticleSubsystemRegistry.h"
#include "render/actors/particles/Characteristics.h"
#include "render/actors/shapes/DebugCircle.h"
#include "utils/Functions.inl"

ParticleSubsystemData Sandbox::wave1(const Array<float>& mt_1)
{
	if (mt_1.Size() != 1)
		throw psregistry_mismatch_error();
	return {
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugCircle(4.0f)),
		CumulativeFunc<ParticleCount>(func_conditional<float, float>(
			make_functor_ptr<true>(&less_than, 0.6f),
			make_functor_ptr<false>(&power_func, std::tuple<float, float>{ 2000.0f, 0.5f }),
			make_functor_ptr<true>(&constant_func<float>, power_func(0.6f, { 2000.0f, 0.5f })))
		),
		func_compose<float, const Particles::CHRSeed&>(Particles::CHR::Seed_waveT(), make_functor_ptr<false>(&linear_func, std::tuple(-0.05f, 0.4f))),
		Particles::CombineSequential({
			Particles::CombineConditionalTimeLessThan(mt_1[0],
				Particles::CHR::SetColorUsingTime(make_functor_ptr<false>(&linear_combo_f<4>, std::array<glm::vec4, 2>{
						glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f }, glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f } })),
				Particles::CHR::SetColorUsingTime(make_functor_ptr<false>(&linear_combo_f<4>, std::array<glm::vec4, 2>{
						glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f + mt_1[0] / (1.0f - mt_1[0]) }, glm::vec4{ 0.0f, 1.0f, 0.0f, -1.0f / (1.0f - mt_1[0])} }))
			),
			Particles::CombineInitialOverTime(
				Particles::CombineSequential({
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					Particles::CHR::SetLocalPositionUsingData(make_functor_ptr([](const glm::vec2& ps) -> Position2D {
						return glm::vec2{ glm::cos(ps.x * 2 * glm::pi<float>()), glm::sin(ps.x * 2 * glm::pi<float>()) } *20.0f * ps.y;
					}), 0, 1),
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					Particles::Characterize(make_functor_ptr([](Particle& p) {
						glm::vec2 vel = linear_combo<2, 2>({ p[0], p[1] }, { glm::vec2{ -1.0f, -1.0f }, glm::vec2{ 2.0f, 0.0f }, glm::vec2{ 0.0f, 2.0f } });
						vel = glm::normalize(vel);
						vel *= glm::vec2{ 200.0f, 100.0f };
						p[0] = vel.x;
						p[1] = vel.y;
					}), 2),
					Particles::CHR::FeedDataRNG(2),
					Particles::CHR::OperateDataUsingSeed(2, make_functor_ptr([](const std::tuple<float&, const Particles::CHRSeed&>& tuple) -> void {
						std::get<0>(tuple) = (1.0f + std::get<0>(tuple) * 0.25f) * (1.0f - 0.4f * glm::pow(std::get<1>(tuple).waveT, 0.2f + std::get<0>(tuple) * 0.25f));
					})),
					Particles::CHR::OperateData(0, make_functor_ptr([](const std::tuple<float&, float>& tuple) { std::get<0>(tuple) *= std::get<1>(tuple); }), 2),
					Particles::CHR::OperateData(1, make_functor_ptr([](const std::tuple<float&, float>& tuple) { std::get<0>(tuple) *= std::get<1>(tuple); }), 2),
				}),
				Particles::CHR::OperateLocalPositionFromVelocityData(0, 1)
			),
			Particles::CHR::SyncAll()
		})
	};
}

ParticleSubsystemData Sandbox::wave2(const Array<float>& p2size)
{
	if (p2size.Size() != 2)
		throw psregistry_mismatch_error();
	return {
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 2}, {1, 2}, {1, 0} }, GL_TRIANGLE_FAN)),
		CumulativeFunc<ParticleCount>(make_functor_ptr<false>(&linear_func, std::tuple<float, float>{ p2size[1] * 0.5f, 0.0f })),
		make_functor_ptr<true>(&constant_func<float, const Particles::CHRSeed&>, 1.5f),
		Particles::CombineSequential({
			Particles::GenSetup(
				Particles::CombineSequential({
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRandSpawnIndex(1),
					Particles::CHR::FeedDataRandBinChoice(2, 1, -1)
				})
			),
			Particles::CHR::FeedDataShiftMod(3, 0, 1.0f),
			Particles::CombineConditionalDataLessThan(
				3, 0.5f,
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(make_functor_ptr<false>(&linear_combo_f<4>, std::array<glm::vec4, 2>{
							glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}, glm::vec4{2.0f, 2.0f, 0.0f, 0.0f} }), 3),
					Particles::CHR::SetLocalScaleUsingData(make_functor_ptr<false>(&linear_combo_f<2>, std::array<glm::vec2, 2>{
							glm::vec2{ 0.0f, 1.0f }, glm::vec2{ 2.0f * p2size[0], 0.0f} }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, float>(
							make_functor_ptr(&cast<float, unsigned int>),
							make_functor_ptr<false>(&linear_combo_f<2>, std::array<glm::vec2, 2>{
									glm::vec2{ -0.5f * p2size[0], -0.5f * p2size[1]}, glm::vec2{ 0.0f, 2.0f } })), 1),
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, const glm::vec2&>(
							vec2_compwise<float, float>(make_functor_ptr(&identity<float>), make_functor_ptr(&cast<float, unsigned int>)),
							make_functor_ptr<false>(&linear_combo<2, 2>, std::array<glm::vec2, 3>{
									glm::vec2{ 0.5f * p2size[0], -0.5f * p2size[1] }, glm::vec2{-2.0f * p2size[0], 0.0f}, glm::vec2{0.0f, 2.0f} })), 3, 1)
					)
				}),
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(make_functor_ptr<false>(&linear_combo_f<4>, std::array<glm::vec4, 2>{
							glm::vec4{2.0f, 2.0f, 1.0f, 1.0f}, glm::vec4{-2.0f, -2.0f, 0.0f, 0.0f} }), 3),
					Particles::CHR::SetLocalScaleUsingData(make_functor_ptr<false>(&linear_combo_f<2>, std::array<glm::vec2, 2>{
							glm::vec2{ 2.0f * p2size[0], 1.0f }, glm::vec2{ -2.0f * p2size[0], 0.0f } }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, const glm::vec2&>(
							vec2_compwise<float, float>(make_functor_ptr(&identity<float>), make_functor_ptr(&cast<float, unsigned int>)),
							make_functor_ptr<false>(&linear_combo<2, 2>, std::array<glm::vec2, 3>{
									glm::vec2{ -1.5f * p2size[0], -0.5f * p2size[1] }, glm::vec2{ 2.0f * p2size[0], 0.0f }, glm::vec2{ 0.0f, 2.0f }})), 3, 1),
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, float>(
							make_functor_ptr(&cast<float, unsigned int>),
							make_functor_ptr<false>(&linear_combo_f<2>, std::array<glm::vec2, 2>{
									glm::vec2{ -0.5f * p2size[0], -0.5f * p2size[1]}, glm::vec2{ 0.0f, 2.0f }})), 1)
					)
				})
			),
			Particles::CHR::SyncAll()
		})
	};
}
