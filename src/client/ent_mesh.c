#include "entity.h"
#include "quark.h"

void entity_init_mesh(quark_t* quark,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_MESH, (vec3){x, y, z}, &entity);
    entity->data.mesh.mesh = mesh_copy(quark, mesh);
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.mesh.materials[i] = NULL;
	}
	entity->data.mesh.is_visible = true;
	entity->data.mesh.is_pickup = false;
	entity->data.mesh.enable_collision = true;
	entity->data.mesh.enable_physics = false;

    *out = entity;
}

void entity_tick_mesh(quark_t* quark, entity_t* entity, entity_mesh_t* mesh) {
	if (mesh->is_pickup) {
		glm_quat(entity->rotation, rad(quark->time * PICKUP_SPIN_RATE), 0.0f, 1.0f, 0.0f);
	}

	if (mesh->enable_physics) {
		float speed = glm_vec3_dot(entity->velocity, entity->velocity);

		if (speed > PHYS_REST_VELOCITY) {			
			vec3 friction_dir;
			glm_vec3_copy(entity->velocity, friction_dir);
			glm_vec3_inv(friction_dir);
			glm_vec3_normalize(friction_dir);

			float mass = 40.0f;
			float normal_force = mass * PHYS_GRAVITY;
			float friction_coeff = 1.0f;

			vec3 friction_force;
			glm_vec3_copy(friction_dir, friction_force);
			glm_vec3_scale(friction_force, normal_force * friction_coeff, friction_force);

			vec3 acceleration;
			glm_vec3_copy(friction_force, acceleration);
			glm_vec3_divs(acceleration, mass, acceleration);

			vec3 friction;
			glm_vec3_copy(acceleration, friction);
			glm_vec3_scale(friction, quark->dt, friction);

			entity->velocity[0] += friction[0];
			entity->velocity[2] += friction[2];

		} else {
			glm_vec3_zero(entity->velocity);
		}

		speed = glm_vec3_dot(entity->velocity, entity->velocity);

		if (speed > 0.001f) {
			ray_t ray;
			glm_vec3_copy(entity->position, ray.origin);
			glm_vec3_copy(entity->velocity, ray.dir);
			glm_vec3_norm(ray.dir);

			float max_distance = speed * quark->dt;
			trace_result_t trace;
			int ignore[] = {entity->id};

			if (phys_line_trace(quark, ray, max_distance, &quark->map.entlist,
				-1, ignore, 1, &trace))
			{
				float vel_dot_normal = glm_vec3_dot(entity->velocity, trace.normal);

				vec3 slide;
				glm_vec3_scale(trace.normal, vel_dot_normal, slide);
				glm_vec3_sub(entity->velocity, slide, slide);

				glm_vec3_copy(slide, entity->velocity);

				vec3 half_size;
				bbox_get_half_size(&entity->local_bbox, half_size);

				entity->position[0] = trace.point[0] + entity->velocity[0] * quark->dt;
				entity->position[1] = trace.point[1] + entity->velocity[1] * quark->dt;
				entity->position[2] = trace.point[2] + entity->velocity[2] * quark->dt;
			}

			vec3 end;
			point_on_ray(ray, speed * quark->dt, end);
			r_add_line(quark, &quark->renderer, entity->position, end, COLOR_RED, 0.0f);

    		//entity->velocity[1] -= PHYS_GRAVITY * quark->dt;
		} else {
    		//entity->velocity[1] = 0.0f;
		}

		vec3 move;
		glm_vec3_copy(entity->velocity, move);
		glm_vec3_scale(move, quark->dt, move);
		glm_vec3_add(entity->position, move, entity->position);
	}
}

void entity_mesh_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot) {
	if (!entity || entity->type != ENTITY_MESH) return;
	
	if (slot >= MAX_MATERIALS) {
		error(quark, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.mesh.materials[slot] = material;
}
