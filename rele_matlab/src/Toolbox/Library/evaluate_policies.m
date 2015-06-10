function J = evaluate_policies ( policies, domain, makeDet )
% Given a set of low-level POLICIES, it returns the corresponding return J
% in the objectives space.
% Set MAKEDET to 1 if you want to make the policies deterministic.

[n_obj, ~, episodes, steps] = settings(domain);

mdpconfig = [domain '_mdpvariables'];
mdp_vars = feval(mdpconfig);
isStochastic = mdp_vars.isStochastic;
if makeDet && ~isStochastic
    episodes = 1;
end

N_pol = numel(policies);
J = zeros(N_pol, n_obj);

%%%%%%%%%%%%
[mexParams, gamma, max_obj] = getMexParams(domain);
%%%%%%%%%%%%

parfor i = 1 : N_pol
    
%     fprintf('Evaluating policy %d of %d ...\n', i, N_pol)
    
    if makeDet
        policy = policies(i).makeDeterministic;
    else
        policy = policies(i);
    end
    
%     [~, J_sample] = collect_samples(domain, episodes, steps, policy);
    
%%%%%%%%%%%%
    mm = mexParams;
    mm.policyParameters = policy.theta;
    [~, J_sample] = collectSamples(domain, episodes, steps, gamma, mm);
    J_sample = mean(J_sample,1) .* abs(max_obj)';
%%%%%%%%%%%%
    
    J(i,:) = J_sample;

end

end
