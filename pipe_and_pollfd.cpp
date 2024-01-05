# include <vector>
# include <map>
# include <iostream>
# include <string>
# include <sstream>
# include <iterator>
# include <arpa/inet.h>
#include <poll.h>

#include <unistd.h>

int	main() {

	int into_cgi[2];
	int	outta_cgi[2];

	if (pipe(into_cgi) == -1) {
		std::cerr << "pipe failure into_cgi" << std::endl; 
		return 1;
	}

	if (pipe(outta_cgi) == -1) {
		std::cerr << "pipe failure outta_cgi" << std::endl; 
		return 1;
	}

	std::cout << "into_cgi[READ_END]: " << into_cgi[0] << std::endl;
	std::cout << "into_cgi[WRITE_END]: " << into_cgi[1] << std::endl;
	std::cout << "outta_cgi[READ_END]: " << outta_cgi[0] << std::endl;
	std::cout << "outta_cgi[WRITE_END]: " << outta_cgi[1] << std::endl;

	pollfd	into_cgi_read = {into_cgi[0], POLLIN, 0};
	pollfd	into_cgi_write = {into_cgi[1], POLLOUT, 0};

	pollfd	outta_cgi_read = {outta_cgi[0], POLLIN, 0};
	pollfd	outta_cgi_write = {outta_cgi[1], POLLOUT, 0};


	std::vector<pollfd> pollfd_vec;

	pollfd_vec.push_back(into_cgi_read);
	pollfd_vec.push_back(into_cgi_write);
	pollfd_vec.push_back(outta_cgi_read);
	pollfd_vec.push_back(outta_cgi_write);

	while (1) {

		poll(&pollfd_vec[0], pollfd_vec.size(), 1000);

		for (std::vector<pollfd>::iterator it = pollfd_vec.begin(); it != pollfd_vec.end(); ++it) {
			if (it->revents & (POLLIN | POLLOUT)) {
				std::cout << "fd " << it->fd << " revent is triggering: ";
				it->revents & POLLIN ? std::cout << "POLLIN" << std::endl : std::cout << "POLLOUT" << std::endl;
			}
		}
	}

	return 0;

}