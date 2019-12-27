#pragma once

#define GEN_5(api, clas) api clas(const clas& other);       \
					api clas(clas&& other) noexcept;   \
					api ~clas();                       \
					api clas& operator=(const clas& other);     \
					api clas& operator=(clas&& other) noexcept; \


#define GEN_default5(api, clas) api clas(const clas& other) = default;       \
						  api clas(clas&& other) = default;            \
						  api ~clas() = default;                       \
						  api clas& operator=(const clas& other) = default;  \
						  api clas& operator=(clas&& other) = default;       \
