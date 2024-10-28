#pragma once

namespace IL2CPP
{
	namespace Helper
	{
		Unity::CComponent* GetMonoBehaviour()
		{
			Unity::il2cppArray<Unity::CGameObject*>* m_Objects = Unity::Object::FindObjectsOfType<Unity::CGameObject>(UNITY_GAMEOBJECT_CLASS);
			for (uintptr_t u = 0U; m_Objects->m_uMaxLength > u; ++u)
			{
				Unity::CGameObject* m_Object = m_Objects->operator[](static_cast<unsigned int>(u));
				if (!m_Object) continue;

				Unity::CComponent* m_MonoBehaviour = m_Object->GetComponentByIndex(UNITY_MONOBEHAVIOUR_CLASS);
				if (m_MonoBehaviour)
					return m_MonoBehaviour;
			}

			return nullptr;
		}
	}
}

void Log(uintptr_t address, const char* name) {
	printf("[LOG] %s: 0x%X\n", name, address);
}

float Distance3D(Unity::Vector3 A, Unity::Vector3 B) {
	return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2) + pow(A.z - B.z, 2));
}

uintptr_t ReadChain(uintptr_t baseAddress, std::vector<uintptr_t>& offsets) {
	uintptr_t address = baseAddress;
	for (int i = 0; i < offsets.size(); i++) {
		address += offsets[i];
		address = *(uintptr_t*)address;
	}

	return address;
}

void PrintVector3(const std::string& label, const Unity::Vector3& vec) {
	std::cout << label << ": ("
		<< vec.x << ", "
		<< vec.y << ", "
		<< vec.z << ")"
		<< std::endl;
}