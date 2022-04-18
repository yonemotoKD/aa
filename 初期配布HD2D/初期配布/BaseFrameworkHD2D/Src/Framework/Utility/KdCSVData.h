#pragma once

struct KdCSVData
{
	KdCSVData() {}
	KdCSVData(const std::string_view filename) { Load(filename); }

	bool Load(const std::string_view filename);

	const std::vector<std::string>& KdCSVData::GetLineData(size_t index);

private:

	void CommaSeparatedValue(const std::string& src, std::vector<std::string>& result);

	std::vector<std::vector<std::string>> m_lineDatas;

	std::string m_filePass;

	static const std::vector<std::string> c_nullDataList;
};