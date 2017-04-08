#pragma once

struct Material
{
	double reflection;
	double density;
	double absorption;
	double pathLoss;

	Material() : Material(0, 0, 0, 0)
	{ }

	Material(double reflection, double density, double absorption, double pathLoss) :
		reflection(reflection), 
		density(density), 
		absorption(absorption), 
		pathLoss(pathLoss)
	{ }

	Material operator*(double by) const
	{
		return Material(
			this->reflection * by,
			this->density * by,
			this->absorption * by,
			this->pathLoss * by);
	}
};