#pragma once

struct Material
{
	const double reflection;
	const double absorption;

	Material(double reflection, double absorption) :
		reflection(reflection),
		absorption(absorption)
	{ }
};