#pragma once

struct Material
{
	const double reflection;
	const double density;
	const double absorption;
	const double pathLoss;

	Material(double reflection, double density, double absorption, double pathLoss) :
		reflection(reflection), 
		density(density), 
		absorption(absorption), 
		pathLoss(pathLoss)
	{ }
};