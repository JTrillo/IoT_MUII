import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet(
	    name = "RequestGraph",
	    urlPatterns = {"/requestGraph"}
)
public class RequestGraph extends HttpServlet{
	private static final long serialVersionUID = 1L;

	@Override
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException{
		
		String node = request.getParameter("node");
		String graph = request.getParameter("graph");
		
		request.setAttribute("node", node);
		RequestDispatcher rd;
		
		switch (graph) {
			case "TEMP":
				rd = request.getRequestDispatcher("/temperature");
				rd.forward(request, response);
				break;
			case "HAIR":
				rd = request.getRequestDispatcher("/humidity_air");
				rd.forward(request, response);
				break;
			case "HGRO":
				rd = request.getRequestDispatcher("/humidity_ground");
				rd.forward(request, response);
				break;
			case "PRES":
				rd = request.getRequestDispatcher("/pressure");
				rd.forward(request, response);
				break;
			case "BRIG":
				rd = request.getRequestDispatcher("/brightness");
				rd.forward(request, response);
				break;
		}
		
	}
	
	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException{
		this.doPost(request, response);
	}
}
