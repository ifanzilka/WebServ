//
// Created by Josephine Beregond on 5/2/22.
//

#include "Response.hpp"

Response::Response(Request &request)
{
	_contentType = "text/html";
	if ((_statusCode = request.getErrorStatus()) == 0)
	{
		std::cout << GREEN"===ZERO CODE==="NORM << std::endl; // TODO: удалить
		_bodySize = 0;
		_reqHeaders = request.getHeaders();
		_method = request.getMethod();
		_url = putDelete(request, _statusCode);
		_reqLocation = request.getLocation();
		_autoindex = _statusCode == 1;
	}
}
