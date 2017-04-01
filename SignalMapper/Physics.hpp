#pragma once

struct Material
{
	double reflection;
	double density;
	double absorption;
	double pathLoss;

	Material(double reflection, double density, double absorption, double pathLoss) :
		reflection(reflection), density(density), absorption(absorption), pathLoss(pathLoss)
	{ }
};