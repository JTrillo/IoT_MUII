import java.io.IOException;

import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;

@WebServlet(
    name = "HelloAppEngine",
    urlPatterns = {"/hello"}
)
public class HelloAppEngine extends HttpServlet {
	private static final long serialVersionUID = 1L;
	
	@Override
	public void doGet(HttpServletRequest request, HttpServletResponse response) 
			throws IOException {

		response.setContentType("text/plain");
		response.setCharacterEncoding("UTF-8");

		response.getWriter().print("Test added!\r\n");

	    DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
	    Entity entrada = new Entity("Vineyard");
	
	    entrada.setProperty("timestamp", "2018-05-29");
	    entrada.setProperty("temperature", 27.4);
	    entrada.setProperty("hum_air", 44.2);
	    entrada.setProperty("hum_ground", 60.8);
	    entrada.setProperty("pressure", 1000);
	    entrada.setProperty("brightness", 700);
	
	    ds.put(entrada);

	    Entity entrada2 = new Entity("Vineyard");

	    entrada2.setProperty("timestamp", "2018-05-30");
	    entrada2.setProperty("temperature", 19.9);
	    entrada2.setProperty("hum_air", 44.5);
	    entrada2.setProperty("hum_ground", 62.8);
	    entrada2.setProperty("pressure", 997);
	    entrada2.setProperty("brightness", 400);
	
	    ds.put(entrada2);
	    
	    Entity entrada3 = new Entity("Vineyard");
		
	    entrada3.setProperty("timestamp", "2018-05-31");
	    entrada3.setProperty("temperature", 25.4);
	    entrada3.setProperty("hum_air", 43.2);
	    entrada3.setProperty("hum_ground", 60.9);
	    entrada3.setProperty("pressure", 1010);
	    entrada3.setProperty("brightness", 700);
	
	    ds.put(entrada3);
	}
}