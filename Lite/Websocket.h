#pragma once

#include <memory>

#include "ConnIO.h"
#include "Address.h"

std::shared_ptr<CConnIO> CreateWebsocket(
	const CAddress& address,
	std::shared_ptr<CConnEventDelegate> delegate);