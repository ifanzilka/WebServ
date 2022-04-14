/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerApi.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmarilli <bmarilli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 00:49:56 by bmarilli          #+#    #+#             */
/*   Updated: 2022/04/14 03:21:00 by bmarilli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_API
# define SERVER_API

namespace ft
{
	class AbstractServerApi
	{
	public:
		virtual	void 	Start() = 0;
		virtual	void	Init() = 0;
    	virtual int 	Create_socket() = 0;
    	virtual int 	Binded() = 0;
    	virtual int 	Listen() = 0;
	protected:
		virtual void	ServerError() = 0;
	};

}

#endif