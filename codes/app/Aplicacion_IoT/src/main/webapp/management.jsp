<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="org.json.JSONObject" %>  
<%@ page import="com.google.appengine.api.users.*" %>
<%@ page import="com.google.appengine.api.datastore.*" %>
<%@ page import="com.google.appengine.api.datastore.Query.Filter" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterPredicate" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterOperator" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
	<title>Users management</title>
	<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
	<!-- Latest compiled and minified CSS -->
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"> 
</head>
<body>
	<div class="row">
		<h2 align="center">Current users</h2><br>
		<div class="table-responsive col-md-offset-3 col-md-6 col-sm-offset-3 col-sm-6">
			<table class="table table-bordered table-striped">
				<thead>
					<tr>
						<th>Email</th>
						<th>Admin</th>
					</tr>
				</thead>
				<tbody>
				<%
				DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
				Query q = new Query("Users");
				PreparedQuery pq = ds.prepare(q);
				for(Entity e : pq.asIterable()){
					String email = e.getProperty("email").toString();
					String admin = e.getProperty("admin").toString();%>
					<tr>
						<td><%=email%></td>
						<td><%=admin%></td>
					</tr><%
				}
				%>
				</tbody>
			</table>
		</div>
	</div>
	<%
	Query q2 = new Query("Requests");
	PreparedQuery pq2 = ds.prepare(q2);
	if(pq2.countEntities()>0){
	%>
	<div class="row">
		<h2 align="center">Current requests</h2><br>
		<div class="table-responsive col-md-offset-3 col-md-6 col-sm-offset-3 col-sm-6">
			<table class="table table-bordered table-striped">
				<thead>
					<tr>
						<th>Email</th>
						<th>Grant access to this account?</th>
					</tr>
				</thead>
				<tbody>
				<%
				for(Entity e : pq2.asIterable()){
					String email = e.getProperty("email").toString();%>
					<tr>
						<td><%=email%></td>
						<td><button type="button" class="btn btn-default col-md-offset-3 col-sm-offset-3"><a href="AcceptRequest?email=<%=email %>">Accept request</a></button></td>
					</tr><%
				}
				%>
				</tbody>
			</table>
		</div>
	</div>
	<br><br>
	<%
	}else{
	%>
	<div class="alert alert-info col-md-offset-3 col-md-6 col-sm-offset-3 col-sm-6">
	  There are not requests now. Check it later.
	</div>
	<%
	}
	%>
	<div>
		<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="index.jsp">Back</a></button>
	</div>
	
	
	<!-- jQuery library -->
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
	<!-- Latest compiled JavaScript -->
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
</body>
</html>