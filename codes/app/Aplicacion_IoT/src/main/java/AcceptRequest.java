import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.PreparedQuery;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.Query.Filter;
import com.google.appengine.api.datastore.Query.FilterOperator;
import com.google.appengine.api.datastore.Query.FilterPredicate;

@WebServlet(
		name = "AcceptRequest",
		urlPatterns = {"/AcceptRequest"}
)
public class AcceptRequest extends HttpServlet{

	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		String email = request.getParameter("email");
		
		DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
		//First, we delete the request from entity 'Requests'
		Filter filter = new FilterPredicate("email",FilterOperator.EQUAL, email);
		Query q = new Query("Requests").setFilter(filter);
		PreparedQuery pq = ds.prepare(q);
		for(Entity e: pq.asIterable()) {
			ds.delete(e.getKey());
		}
		//Then, we create the user with non-admin permissions (entity 'Users')
		Entity entity = new Entity("Users");
		entity.setProperty("email", email);
		entity.setProperty("admin", "No");
		ds.put(entity);
		
		request.getRequestDispatcher("/management.jsp").forward(request, response);
	}
	
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		this.doGet(request, response);
	}
}
