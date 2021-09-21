#pragma once

template<typename _t>
std::string tos(const _t& numb)
{
	std::stringstream buf;
	if (numb >= 0) buf << numb;
	else           buf << " "; // need space for UpdateMesh

	return buf.str();
}
